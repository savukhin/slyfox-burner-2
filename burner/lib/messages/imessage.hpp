#pragma once

#include "crsf_protocol.hpp"

class IMessage {
public:
    virtual uint8_t get_id();
    
    virtual void* New(void* source);
};
