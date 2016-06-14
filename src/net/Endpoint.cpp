#include "scraps/net/Endpoint.h"

namespace scraps {
namespace net {

socklen_t Endpoint::getSockAddr(sockaddr_storage* storage) const {
    memset(storage, 0, sizeof(*storage));

    if (address().is_v4()) {
        auto addr        = reinterpret_cast<sockaddr_in*>(storage);
        addr->sin_family = AF_INET;
        auto bytes = address().to_v4().to_bytes();
        static_assert(sizeof(addr->sin_addr) == sizeof(bytes), "size mismatch");
        memcpy(&addr->sin_addr, &bytes, sizeof(bytes));
        addr->sin_port = htons(port());
        return sizeof(*addr);
    } else if (address().is_v6()) {
        auto ipv6         = address().to_v6();
        auto addr         = reinterpret_cast<sockaddr_in6*>(storage);
        addr->sin6_family = AF_INET6;
        auto bytes = ipv6.to_bytes();
        static_assert(sizeof(addr->sin6_addr) == sizeof(bytes), "size mismatch");
        memcpy(&addr->sin6_addr, &bytes, sizeof(bytes));
        addr->sin6_port     = htons(port());
        addr->sin6_scope_id = htonl(ipv6.scope_id());
        return sizeof(*addr);
    }

    SCRAPS_ASSERT(false);
    return 0;
}

}} // namespace scraps::net
