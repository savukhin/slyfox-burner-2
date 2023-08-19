#pragma once

#include <stdint.h>

class ISerial {
public:
    virtual uint8_t* readByte() = 0;
    virtual void writeByte(uint8_t data) = 0;
    virtual void writeBytes(uint8_t *data, long length) = 0;
};
