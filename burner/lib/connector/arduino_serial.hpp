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
        if (Serial.available() == 0) {
            // Serial.println("una                                                                                                                                                                                                                                                                                                                                                                                                                          vailable");
            return nullptr;
        }
        
        uint8_t *buf = new uint8_t[1] { Serial.read() };
        // Serial.println("read 1 " + String((int)buf[0]) + "' final");

        return buf;
    }

    void writeByte(uint8_t data) {
        Serial.write(data);
    }

    void writeBytes(uint8_t *data, long length) override {
        Serial.write(data, length);
    }

    void close() override {}
};
#endif