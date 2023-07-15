#pragma once

#include "iserial.hpp"
#include <Arduino.h>

class ArduinoSerial : public ISerial {
public:
    ArduinoSerial(unsigned int baud) {
        Serial.begin(baud);
    }

    uint8_t* readByte() override {
        uint8_t* buf;

        if (Serial.available() == 0)
            return nullptr;
        
        Serial.readBytes(buf, 1);

        return buf;
    }

    void writeByte(uint8_t data) {
        Serial.write(data);
    }

    void writeBytes(uint8_t *data, long length) override {
        Serial.write(data, length);
    }
};