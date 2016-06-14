#pragma once

#include "scraps/config.h"

#include <mutex>
#include <condition_variable>
#include <thread>

#include <curl/curl.h>

namespace scraps {
namespace net {

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
    bool error();

    /**
    * @return true if the request is complete
    */
    bool isComplete();

    /**
    * @return the response status code if the request is complete
    */
    unsigned int responseStatus();

    /**
    * @return the response body if the request is complete
    */
    std::string responseBody();

private:
    // no copying
    HTTPRequest(const HTTPRequest&) = delete;
    HTTPRequest& operator=(const HTTPRequest&) = delete;

    std::string _caBundlePath;
    std::string _pinnedKey;

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

    static size_t CURLWriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
};

}} // namespace scraps::net
