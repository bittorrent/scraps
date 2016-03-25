#pragma once

#include "scraps/config.h"
#include "scraps/types.h"

#include <unordered_map>

namespace scraps {

class HTTPConnection {
public:
    enum ResultType {
        kResultUnknown = -1,
        kResultSuccess,
        kResultReceiveError,
        kResultSocketClosedByPeer,
        kResultMalformedRequest,
        kResultErrorWaitingOnSocket,
        kResultTimeout,
        kResultSendError,
    };

    static constexpr const char* kMimeType_TextPlain       = "text/plain";
    static constexpr const char* kMimeType_ApplicationJSON = "application/json";

    HTTPConnection(int socket, const std::chrono::microseconds& timeout = 15s);
    virtual ~HTTPConnection() {}

    void run();
    void cancel();

    ResultType result() const { return _result; }

    typedef std::unordered_map<std::string, std::string> HeaderMap;

    struct Request {
        std::string method;
        std::string resource;
        HeaderMap headers;
        std::string body;
        std::unordered_map<std::string, std::string> get;
        std::string path;
    };

    /**
    * Implement this method to perform request handling. Do not call this directly.
    *
    * @param request the request to be handled
    */
    virtual void handleRequest(const Request& request) = 0;

    /**
    * Sends a response to a request. Should only be called from within a handleRequest implementation.
    *
    * @param status the http status line (e.g. "HTTP/1.1 200 OK")
    * @param body the body of the response
    * @param bodyLength the length of the response's body
    * @param mimetype the mimetype of the response
    */
    void sendResponse(const char* status,
                      const void* body,
                      size_t bodyLength,
                      const char* mimetype = kMimeType_TextPlain);

private:
    HTTPConnection(const HTTPConnection& other) = delete;
    HTTPConnection& operator=(const HTTPConnection& other) = delete;

    int _socket;
    ResultType _result;

    std::chrono::microseconds _timeout;

    std::string _request;

    /**
    * @return 0 if the request was successfully parsed, -1 if the request is malformed, 1 if the request is incomplete
    */
    static int ParseRequest(const std::string& request, Request* parsed);

    static std::unordered_map<std::string, std::string> ParseQueryString(const std::string& queryString);
};

} // namespace scraps
