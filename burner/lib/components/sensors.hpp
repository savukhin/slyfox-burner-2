#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <stdint.h>

#include "component_interfaces.hpp"

class PhotoResistor : public ISensor {
private:
    uint8_t pin_;
public:
    PhotoResistor(uint8_t pin): pin_(pin) {
        #ifdef ARDUINO
        pinMode(pin, INPUT);
        #endif
    }

    uint16_t read() {
        uint16_t result;
        #ifdef ARDUINO
        result = analogRead(pin_);
        #endif
        return result;
    }
};
