#pragma once

#include "iserial.hpp"

#ifdef ARDUINO
#include <Arduino.h>

class ArduinoSerial : public ISerial {
HardwareSerial *serial_;

public:
    ArduinoSerial(unsigned int baud, HardwareSerial *serial) {
        this->serial_ = serial;
        this->serial_->begin(baud);
    }

    uint8_t* readByte() override {
        if (this->serial_->available() == 0) {
            // Serial.println("una                                                                                                                                                                                                                                                                                                                                                                                                                          vailable");
            return nullptr;
        }
        
        uint8_t *buf = new uint8_t[1] { (uint8_t)this->serial_->read() };
        // Serial.println("read 1 " + String((int)buf[0]) + "' final");

        return buf;
    }

    void writeByte(uint8_t data) {
        // printf("%d",data);
        // print(data);
        // Serial.write(data);
        // Serial.flush();
        this->serial_->write(data);
        this->serial_->flush();
    }

    void writeBytes(uint8_t *data, long length) override {
        // for (int i = 0; i < length; i++)
        //     this->writeByte(data[i]);

        this->serial_->write(data, length);
        this->serial_->flush();

        // printf("%s", (char*)data);
        // printf("writing bytes...");
        // printf("%.*s", length, (char*)data);
        // printf("writing bytes %.*s", length, (char*)data);
        // Serial.write(data, length);
        // Serial.flush();
    }

    void close() override {}
};
#endif