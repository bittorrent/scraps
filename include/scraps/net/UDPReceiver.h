#pragma once

#include "scraps/config.h"

#include "scraps/net/Endpoint.h"

namespace scraps {
namespace net {

class UDPReceiver {
public:
    virtual ~UDPReceiver() {}

    virtual void receiveUDP(const Endpoint& sender, const void* data, size_t length) = 0;
};

}} // namespace scraps::net
