#pragma once

#include "iserial.hpp"

#ifdef ARDUINO
#include <Arduino.h>

class ArduinoSerial : public ISerial {
public:
    ArduinoSerial(unsigned int baud) {
        Serial.begin(baud);
    }

    uint8_t* readByte() override {
        uint8_t* buf;

        if (Serial.available() == 0) {
            Serial.println("unavailable");
            return nullptr;
        }
        
        size_t size = Serial.readBytes(buf, 1);
        Serial.println("read size " + String((int)size) + "' end");

        if (size == 0) return nullptr;
        Serial.println("read 1 " + String((int)buf[0]) + "' ends");

        return buf;
    }

    void writeByte(uint8_t data) {
        Serial.write(data);
    }

    void writeBytes(uint8_t *data, long length) override {
        Serial.write(data, length);
    }
};
#endif