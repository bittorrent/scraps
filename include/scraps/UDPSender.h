#pragma once

#include "scraps/config.h"

#include "scraps/types.h"

namespace scraps {

class UDPSender {
public:
    virtual ~UDPSender() {}
    
    virtual bool send(const UDPEndpoint& destination, const void* data, size_t length) = 0;
};

} // namespace scraps
