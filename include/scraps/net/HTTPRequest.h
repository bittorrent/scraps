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
#pragma once

#include <scraps/config.h>

#include <curl/curl.h>

#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace scraps::net {

/**
* Executes HTTP requests asyncronously. This class is thread-safe.
*
* Constructing an instance sends the request immediately. When the connection is closed
* and the request has either completed or failed, isComplete() will return true or
* error() will return an error code.
*/
class HTTPRequest {
public:
    /**
    * Creates an uninitiated HTTP request.
    */
    HTTPRequest() = default;

    /**
    * Creates and immediately initiates an HTTP request with the given parameters.
    */
    template <typename... Args>
    HTTPRequest(Args&&... args) { initiate(std::forward<Args>(args)...); }

    /**
    * Upon destruction, the request is aborted.
    */
    ~HTTPRequest();

    /**
    * @param caBundlePath path to the ca bundle that should be used to verify secure connections.
    *                     if omitted, the system default is used
    */
    void setCABundlePath(std::string caBundlePath) { _caBundlePath = std::move(caBundlePath); }

    /**
    * @param pinnedKey either a path to a public key to pin or a base64 sha256 hash preceded by "sha256//"
    */
    void setPinnedKey(std::string pinnedKey) { _pinnedKey = std::move(pinnedKey); }

    /**
    * For debugging / testing only. Never use this in production.
    */
    void disablePeerVerification() { _disablePeerVerification = true; }

    /**
    * @param url the url to request
    * @param body the optional request body. if given, the request will be a POST request
    * @param bodyLength the length of the body. if omitted, body is assumed to be a null-terminated string
    * @param headers additional headers to send in the request
    */
    void initiate(const std::string& url,
                  const void* body                        = nullptr,
                  size_t bodyLength                       = 0,
                  const std::vector<std::string>& headers = {});

    /**
    * Blocks until the request completes or fails.
    */
    void wait();

    /**
    * Aborts the request.
    */
    void abort();

    /**
    * @return true if the request encountered an error
    */
    bool error() const;

    /**
    * @return true if the request is complete
    */
    bool isComplete() const;

    /**
    * @return the response status code if the request is complete
    */
    unsigned int responseStatus() const;

    /**
    * @return the response body if the request is complete
    */
    std::string responseBody() const;

    /**
    * @return the response headers if the request is complete
    */
    std::vector<std::string> responseHeaders() const;

    /**
    * @return an array of values for the given case-insensitive header name
    */
    std::vector<std::string> responseHeaders(const std::string& name) const;

    /**
    * @return mapping of cookies provided with the response via Set-Cookie
    * including optional cookie directives such as Max-Age=0.
    */
    std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>>
    responseCookies() const;

private:
    // no copying
    HTTPRequest(const HTTPRequest&) = delete;
    HTTPRequest& operator=(const HTTPRequest&) = delete;

    std::string _caBundlePath;
    std::string _pinnedKey;
    bool _disablePeerVerification = false;

    mutable std::mutex _mutex;
    std::condition_variable _condition;

    std::thread _worker;

    CURL* _curl                 = nullptr;
    CURLM* _curlMultiHandle     = nullptr;
    curl_slist* _curlHeaderList = nullptr;

    std::string _body;
    bool _isComplete    = false;
    bool _shouldAbort   = false;
    bool _error         = false;
    int _responseStatus = 0;
    std::string _responseBody;
    std::vector<std::string> _responseHeaders;

    static size_t CURLWriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
    static size_t CURLHeaderWriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
};

namespace detail {
    std::vector<std::string> HeaderValuesFromHTTPResponse(const std::vector<std::string>& headers, const std::string& name);

    // cookie key to value, directives
    std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>>
    CookiesFromHTTPResponseHeaders(const std::vector<std::string>& headers);
} // namespace detail

} // namespace scraps::net
