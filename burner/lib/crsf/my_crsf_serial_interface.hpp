#pragma once

#include <stdint.h>
#include <uleb128.h>

class IHeader {
public:
    virtual int get_msg_type_id() = 0;
    virtual int get_request_id() = 0;
    virtual int get_length() = 0;
    virtual int get_sum() = 0;


    virtual void* get_payload() = 0;
    virtual uint8_t* to_bytes() = 0;
};

class IByteHandler
{
public:
    virtual IHeader* handleByte(uint8_t b) = 0;
};
