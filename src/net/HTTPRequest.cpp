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
#include <scraps/net/HTTPRequest.h>

#include <scraps/logging.h>
#include <scraps/thread.h>
#include <scraps/utility.h>
#include <scraps/net/curl.h>

#include <gsl.h>

namespace scraps::net {

namespace detail {

std::vector<std::string> HeaderValuesFromHTTPResponse(const std::vector<std::string>& headers, const std::string& name) {
    std::vector<std::string> ret;
    for (auto& header : headers) {
        auto it = std::search(header.begin(), header.end(), name.begin(), name.end(), [](char a, char b) { return std::toupper(a) == std::toupper(b); });
        if (it == header.begin()) {
            std::advance(it, name.size());
            if (it == header.end() || *(it++) != ':') { continue; }
            auto raw = std::string{it, header.end()};
            auto value = gsl::to_string(Trim(gsl::string_span<>{raw}));
            ret.emplace_back(std::move(value));
        }
    }
    return ret;
}

std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>>
CookiesFromHTTPResponseHeaders(const std::vector<std::string>& headers) {
    // from https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Set-Cookie
    // Set-Cookie: <cookie-name>=<cookie-value>
    // Set-Cookie: <cookie-name>=<cookie-value>; Expires=<date>
    // Set-Cookie: <cookie-name>=<cookie-value>; Max-Age=<non-zero-digit>
    // Set-Cookie: <cookie-name>=<cookie-value>; Domain=<domain-value>
    // Set-Cookie: <cookie-name>=<cookie-value>; Path=<path-value>
    // Set-Cookie: <cookie-name>=<cookie-value>; Secure
    // Set-Cookie: <cookie-name>=<cookie-value>; HttpOnly
    //
    // Set-Cookie: <cookie-name>=<cookie-value>; SameSite=Strict
    // Set-Cookie: <cookie-name>=<cookie-value>; SameSite=Lax
    //
    // // Multiple directives are also possible, for example:
    // Set-Cookie: <cookie-name>=<cookie-value>; Domain=<domain-value>; Secure; HttpOnly

    auto isRFC2616TokenCharacter = [](auto c) {
        //
        // alphanumeric and any of !#$%&'*+-.^_`|~
        return ('a' <= c && c <= 'z')
            || ('A' <= c && c <= 'Z')
            || ('0' <= c && c <= '9')
            || c == '!'
            || c == '#'
            || c == '$'
            || c == '%'
            || c == '&'
            || c == '\''
            || c == '*'
            || c == '+'
            || c == '-'
            || c == '.'
            || c == '^'
            || c == '_'
            || c == '`'
            || c == '|'
            || c == '~'
        ;
    };

    auto keyIsRFC2616Token = [&](const auto& key) {
        return !key.empty() && std::find_if(key.begin(), key.end(), [&](auto& c) {
                return !isRFC2616TokenCharacter(c);
            }) == key.end();
    };

    auto isRFC6265CookieOctet = [](auto c) {
        // cookie-octet   = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E
        //                ; US-ASCII characters excluding CTLs,
        //                ; whitespace DQUOTE, comma, semicolon,
        //                ; and backslash
        return c == 0x21
            || (0x23 <= c && c <= 0x2B)
            || (0x2D <= c && c <= 0x3A)
            || (0x3C <= c && c <= 0x5B)
            || (0x5D <= c && c <= 0x7E)
        ;
    };

    auto valueIsRFC6265CookieValue = [&](const auto& value) {
        return std::find_if(value.begin(), value.end(), [&](auto c) {
            return !isRFC6265CookieOctet(c);
        }) == value.end();
    };

    // " asdf; qwer; 1234" to {"asdf", "qwer", "1234"}
    auto splitDirectives = [](auto&& str) -> std::vector<std::string> {
        std::vector<std::string> directives;
        scraps::Split(str.begin(), str.end(), std::back_inserter(directives), ';');
        for (auto& d : directives) {
            if (d.empty() || d[0] != ' ') {
                return {};
            }
            d.erase(d.begin()); // trim leading whitespace
        }
        return directives;
    };

    std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>> cookies;
    for (auto& h : HeaderValuesFromHTTPResponse(headers, "Set-Cookie")) {
        auto equals = h.find('=');
        if (equals == std::string::npos) {
            return {};
        }
        auto key = h.substr(0, equals);

        if (!keyIsRFC2616Token(key)) {
            return {};
        }


        auto value = h.substr(equals + 1);
        std::vector<std::string> directives;


        auto semicolon = value.find(';');
        if (semicolon != std::string::npos) {
            directives = splitDirectives(value.substr(semicolon + 1));
            value = value.substr(0, semicolon);
        }

        if (!value.empty() && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }

        if (!valueIsRFC6265CookieValue(value)) {
            return {};
        }

        cookies[key] = std::make_pair(value, directives);
    }
    return cookies;
}
} // namespace detail

HTTPRequest::~HTTPRequest() {
    abort();

    if (_curl) {
        curl_multi_remove_handle(_curlMultiHandle, _curl);
        curl_multi_cleanup(_curlMultiHandle);
        curl_easy_cleanup(_curl);
    }

    if (_curlHeaderList) {
        curl_slist_free_all(_curlHeaderList);
    }
}

void HTTPRequest::initiate(const std::string& url, const void* body, size_t bodyLength, const std::vector<std::string>& headers) {
    if (!CURLIsInitialized()) {
        SCRAPS_LOG_WARNING("cURL may not be initialized properly. You should call scraps::net::InitializeCURL on startup.");
    }

    _curl = curl_easy_init();
    _curlMultiHandle = curl_multi_init();

    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(_curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1);

    for (auto& header : headers) {
        _curlHeaderList = curl_slist_append(_curlHeaderList, header.c_str());
    }
    if (_curlHeaderList) {
        curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _curlHeaderList);
    }

    if (_disablePeerVerification) {
        curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0);
    }

    if (body) {
        curl_easy_setopt(_curl, CURLOPT_POST, 1);

        if (bodyLength) {
            _body.assign((const char*)body, bodyLength);
        } else {
            _body.assign((const char*)body);
        }

        curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, _body.data());
        curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, _body.size());
    }

    if (!_caBundlePath.empty()) {
        auto status = curl_easy_setopt(_curl, CURLOPT_CAINFO, _caBundlePath.c_str());
        if (status != CURLE_OK) {
            SCRAPS_LOGF_ERROR("error setting ca bundle (status = %d)", status);
            _error = true;
            return;
        }
    }

    if (!_pinnedKey.empty()) {
        auto status = curl_easy_setopt(_curl, CURLOPT_PINNEDPUBLICKEY, _pinnedKey.c_str());
        if (status != CURLE_OK) {
            SCRAPS_LOGF_ERROR("error pinning public key (status = %d)", status);
            _error = true;
            return;
        }
    }

    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &CURLWriteCallback);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);

    curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, &CURLHeaderWriteCallback);
    curl_easy_setopt(_curl, CURLOPT_HEADERDATA, this);

    curl_multi_add_handle(_curlMultiHandle, _curl);

    _worker = std::thread([&] {
        SetThreadName("HTTPRequest");
        std::unique_lock<std::mutex> lock{_mutex};

        int stillRunning = 1;
        while (stillRunning) {
            auto status = curl_multi_perform(_curlMultiHandle, &stillRunning);
            if (status != CURLM_OK) {
                _error = true;
                break;
            }

            int n = 0;
            while (auto msg = curl_multi_info_read(_curlMultiHandle, &n)) {
                if (msg->msg == CURLMSG_DONE && msg->data.result) {
                    SCRAPS_LOGF_WARNING("curl result code %d", msg->data.result);
                }
            }

            long timeoutMilliseconds = -1;
            curl_multi_timeout(_curlMultiHandle, &timeoutMilliseconds);
            constexpr long defaultTimeoutMilliseconds = 10;
            if (timeoutMilliseconds < 0) {
                timeoutMilliseconds = defaultTimeoutMilliseconds;
            } else {
                timeoutMilliseconds = std::min(timeoutMilliseconds, defaultTimeoutMilliseconds);
            }

            if (_shouldAbort) { break; }
            _condition.wait_for(lock, std::chrono::milliseconds(timeoutMilliseconds));
            if (_shouldAbort) { break; }
        }

        if (!_error && !_shouldAbort) {
            long responseCode = 0;
            curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &responseCode);

            if (!responseCode) {
                _error = true;
            } else {
                _responseStatus = responseCode;
                _isComplete = true;
            }
        }
    });
}

void HTTPRequest::wait() {
    if (_worker.joinable()) {
        _worker.join();
    }
}

void HTTPRequest::abort() {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _shouldAbort = true;
    }
    _condition.notify_all();
    wait();
}

bool HTTPRequest::error() const {
    std::lock_guard<std::mutex> lock{_mutex};
    return _error;
}

bool HTTPRequest::isComplete() const {
    std::lock_guard<std::mutex> lock{_mutex};
    return _isComplete;
}

unsigned int HTTPRequest::responseStatus() const {
    std::lock_guard<std::mutex> lock{_mutex};
    return _responseStatus;
}

std::string HTTPRequest::responseBody() const {
    std::lock_guard<std::mutex> lock{_mutex};
    return _responseBody;
}

std::vector<std::string> HTTPRequest::responseHeaders() const {
    std::lock_guard<std::mutex> lock{_mutex};
    return _responseHeaders;
}

std::vector<std::string> HTTPRequest::responseHeaders(const std::string& name) const {
    return detail::HeaderValuesFromHTTPResponse(responseHeaders(), name);
}

std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>>
HTTPRequest::responseCookies() const {
    return detail::CookiesFromHTTPResponseHeaders(responseHeaders());
}

size_t HTTPRequest::CURLWriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto request = reinterpret_cast<HTTPRequest*>(userdata);
    request->_responseBody.append(ptr, size * nmemb);
    return size * nmemb;
}

size_t HTTPRequest::CURLHeaderWriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto request = reinterpret_cast<HTTPRequest*>(userdata);
    request->_responseHeaders.emplace_back(ptr, size * nmemb);
    return size * nmemb;
}

} // namespace scraps::net
