#pragma once

#include "crsf_protocol.hpp"

class IByteHandler
{
public:
    virtual crsf_header_t* handleByte(uint8_t b);
};
