#pragma once

#include "../crsf/crsf_protocol.hpp"

class IMessage {
public:
    virtual uint8_t get_id() const = 0;
    virtual void fromBytes(void *bytes) = 0;
    virtual void* toBytes() const = 0;
    virtual long long getByteLen() const = 0;
};
