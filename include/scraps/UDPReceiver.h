#pragma once

#include "scraps/config.h"

#include "scraps/types.h"

namespace scraps {

class UDPReceiver {
public:
    virtual ~UDPReceiver() {}
    
    virtual void receiveUDP(const UDPEndpoint& sender, const void* data, size_t length) = 0;
};

} // namespace scraps
