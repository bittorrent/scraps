/**
* Copyright 2016 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "scraps/net/HTTPConnection.h"

#include "scraps/chrono.h"
#include "scraps/logging.h"
#include "scraps/utility.h"
#include "scraps/URL.h"
#include "scraps/net/utility.h"

#include <unistd.h>

namespace scraps {
namespace net {

HTTPConnection::HTTPConnection(int socket, const std::chrono::microseconds& timeout)
    : _socket(socket), _result(kResultUnknown), _timeout(timeout) {}

void HTTPConnection::run() {
    SetBlocking(_socket, false);

    while (true) {
        char buffer[512];
        auto bytes = recv(_socket, buffer, sizeof(buffer), 0);

#if HAVE_LIBWS2_32
        if (!(bytes == -1 && SocketError() == WSAEWOULDBLOCK)) {
#else
        if (!(bytes == -1 && SocketError() == EWOULDBLOCK)) {
#endif
            if (bytes < 0) {
                _result = kResultReceiveError;
                SCRAPS_LOGF_ERROR("error receiving from http socket (errno = %d)", SocketError());
                break;
            }

            if (!bytes) {
                _result = kResultSocketClosedByPeer;
                SCRAPS_LOGF_ERROR("http socket closed by peer");
                break;
            }

            _request.append(buffer, bytes);

            Request request;
            int ret = ParseRequest(_request, &request);
            if (ret == 0) {
                // request completed
                handleRequest(request);
                break;
            } else if (ret == -1) {
                // request malformed
                _result = kResultMalformedRequest;
                SCRAPS_LOGF_ERROR("received malformed http request");
                break;
            }
        }

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(_socket, &rfds);

        auto timeout  = ToTimeval(_timeout);
        const auto rc = ::select(_socket + 1, &rfds, nullptr, nullptr, &timeout);
        if (rc == -1) {
            _result = kResultErrorWaitingOnSocket;
            SCRAPS_LOGF_ERROR("error waiting on http socket (errno = %d)", SocketError());
            break;
        }

        if (rc == 0) {
            _result = kResultTimeout;
            SCRAPS_LOGF_ERROR("request timed out on http socket");
            break;
        }
    }

    ShutdownAndCloseTCPSocket(_socket);
    _socket = -1;
}

void HTTPConnection::cancel() {
    // TODO
}

void HTTPConnection::sendResponse(const char* status, const void* body, size_t bodyLength, const char* mimetype) {
    auto response = Formatf(
        "%s\r\nContent-Length: %u\r\nContent-Type: %s\r\nConnection: close\r\n\r\n", status, bodyLength, mimetype);
    if (bodyLength) {
        response.append((const char*)body, bodyLength);
    }
    const auto rc = send(_socket, response.data(), response.size(), 0);
    _result       = rc >= 0 ? kResultSuccess : kResultSendError;
}

int HTTPConnection::ParseRequest(const std::string& request, HTTPConnection::Request* parsed) {
    size_t begin = 0;

    bool parsedLeadLine = false;

    while (true) {
        size_t end = request.find_first_of("\r\n", begin);

        if (end == std::string::npos) {
            // didn't complete parsing, didn't find another newline. the request probably isn't complete
            return 1;
        }

        if (end == begin) {
            // empty line. must be the end of the header. the rest is the body
            begin        = request.find_first_not_of(request[end] == '\n' ? '\r' : '\n', end + 1);
            parsed->body = (begin == std::string::npos ? "" : request.substr(begin));
            auto it = parsed->headers.find("Content-Length");
            if (it != parsed->headers.end()) {
                size_t contentLength = atoi(it->second.c_str());
                if (parsed->body.size() < contentLength) {
                    return 1;
                } else if (parsed->body.size() > contentLength) {
                    return -1;
                }
            }
            // successfully read, fill in the rest of the field
            size_t q    = parsed->resource.find('?');
            parsed->get = URL::ParseQuery(
                q == std::string::npos || q + 1 >= parsed->resource.size() ? "" : parsed->resource.substr(q + 1));
            parsed->path = q == std::string::npos ? parsed->resource : parsed->resource.substr(0, q);
            return 0;
        }

        std::string line = request.substr(begin, end - begin);

        if (!parsedLeadLine) {
            size_t pos   = 0;
            size_t space = line.find(' ', pos);
            if (space == std::string::npos || space == pos) {
                return -1;
            }
            parsed->method = line.substr(pos, space - pos);

            pos   = space + 1;
            space = line.find(' ', pos);
            if (space == std::string::npos || space == pos) {
                return -1;
            }
            parsed->resource = line.substr(pos, space - pos);

            pos = space + 1;
            if (pos >= line.size()) {
                return -1;
            }
            if (line.substr(pos) != "HTTP/1.1") {
                return -1;
            }

            parsedLeadLine = true;
        } else {
            size_t colon = line.find(':');
            if (colon == std::string::npos) {
                return -1;
            }
            size_t value = line.find_first_not_of(' ', colon + 1);
            if (value == std::string::npos) {
                return -1;
            }

            parsed->headers[line.substr(0, colon)] = line.substr(value);
        }

        begin = request.find_first_not_of(request[end] == '\n' ? '\r' : '\n', end + 1);

        if (begin == std::string::npos) {
            // if there's nothing after this line, the request probably isn't complete
            return 1;
        }
    }

    return -1;
}

}} // namespace scraps::net
