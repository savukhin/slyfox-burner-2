#pragma once

#include <stdint.h>
#include "../messages/imessage.hpp"

class IHeader {
public:
    virtual int get_msg_type_id() = 0;
    virtual long long get_request_id() = 0;
    virtual int get_frame_length() = 0;
    virtual int get_total_length() = 0;
    virtual int get_sum() = 0;


    virtual void* get_payload() = 0;
    virtual uint8_t* to_bytes() = 0;

    virtual ~IHeader() = default;
};

class IByteHandler
{
public:
    virtual void reset() = 0;

    virtual IHeader* handleByte(uint8_t b) = 0;
    // virtual IHeader* makePacket(uint8_t b) = 0;
    virtual IHeader *makePacket(const IMessage &msg, long long req_id) = 0;
    // virtual IHeader *makePacket(Req request_id, Len len, Msg msg_type_id, const void *payload) = 0;

    virtual ~IByteHandler() = default;
};
