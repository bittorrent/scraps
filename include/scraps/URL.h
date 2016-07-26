#pragma once

#include "scraps/config.h"

#include <unordered_map>

namespace scraps {

class URL {
public:
    explicit URL(const std::string& url) : _url{url} {}

    const std::string& str() const { return _url; }

    std::string protocol() const;

    std::string host() const;

    /**
    * The resource string. E.g. for "google.com/thing/a?q", "/thing/a?q" is returned.
    */
    std::string resource() const;

    /**
    * The path component. E.g. for "google.com/thing/a?q", "/thing/a" is returned.
    */
    std::string path() const;

    /**
    * The query string. E.g. for "google.com/thing/a?q", "q" is returned.
    */
    std::string query() const;

    /**
    * The port. This may be inferred from the protocol or 0 if unknown.
    */
    uint16_t port() const;

    static std::unordered_map<std::string, std::string> ParseQuery(const std::string& query);

private:
    std::string _url;
};

} // namespace scraps
