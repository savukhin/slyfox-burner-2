#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <stdint.h>

#include "component_interfaces.hpp"

class Ignitor : public IIgnitor {
private:
    uint8_t pin_;
    bool started_;
public:
    Ignitor(uint8_t pin): pin_(pin) {
        #ifdef ARDUINO
        pinMode(pin, OUTPUT);
        #endif
    }

    bool ignite() {
        if (this->started_) return false;
        #ifdef ARDUINO
        digitalWrite(pin_, HIGH);
        #endif
        return true;
    }

    bool stop() {
        if (!this->started_) return false;
        #ifdef ARDUINO
        digitalWrite(pin_, LOW);
        #endif
        return true;
    }
};
