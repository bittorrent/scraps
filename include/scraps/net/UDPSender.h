#pragma once

#include "scraps/config.h"

#include "scraps/net/Endpoint.h"

namespace scraps {
namespace net {

class UDPSender {
public:
    virtual ~UDPSender() {}

    virtual bool send(const Endpoint& destination, const void* data, size_t length) = 0;
};

}} // namespace scraps::net
