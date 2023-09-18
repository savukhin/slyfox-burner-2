#pragma once

#include "../crsf/crsf_protocol.hpp"

class IMessage {
public:
    virtual uint8_t get_id() const { return 0; };
    virtual void fromBytes(void *) {};
    virtual void* toBytes() const { return nullptr; };
    virtual long long getByteLen() const { return 10; };

    IMessage() = default;
    IMessage(const IMessage*) {};

    virtual ~IMessage() = default;
};
