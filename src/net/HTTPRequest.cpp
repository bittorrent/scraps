#include "scraps/net/HTTPRequest.h"

#include "scraps/logging.h"
#include "scraps/curl.h"
#include "scraps/thread.h"

#include <gsl.h>

namespace scraps {
namespace net {

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
    SCRAPS_ASSERT(CURLIsThreadSafe() && "You need to call InitCURLThreadSafety upon application startup.");

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

bool HTTPRequest::error() {
    std::lock_guard<std::mutex> lock{_mutex};
    return _error;
}

bool HTTPRequest::isComplete() {
    std::lock_guard<std::mutex> lock{_mutex};
    return _isComplete;
}

unsigned int HTTPRequest::responseStatus() {
    std::lock_guard<std::mutex> lock{_mutex};
    return _responseStatus;
}

std::string HTTPRequest::responseBody() {
    std::lock_guard<std::mutex> lock{_mutex};
    return _responseBody;
}

size_t HTTPRequest::CURLWriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto request = reinterpret_cast<HTTPRequest*>(userdata);
    request->_responseBody.append(ptr, size * nmemb);
    return size * nmemb;
}

}} // namespace scraps::net
