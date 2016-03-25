#pragma once

#include "scraps/config.h"

namespace scraps {

class URL {
public:
    URL(const std::string& url) : _url{url} {}

    const std::string& toString() const { return _url; }

    std::string protocol() const;

    std::string host() const;

    /**
    * The resource path. E.g. for "google.com/thing/a?q", "/thing/a?q" is returned.
    */
    std::string resource() const;

    /**
    * The port. This may be inferred from the protocol or 0 if unknown.
    */
    uint16_t port() const;

private:
    std::string _url;
};

} // namespace scraps
