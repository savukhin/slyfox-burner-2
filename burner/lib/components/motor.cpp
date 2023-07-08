#pragma once

#include "component_interfaces.hpp"

#include <stdexcept>

#if defined(ARDUINO)        
#include <Arduino.h>
#include <FreeRTOSConfig.h>

class ArduinoMotor : public IMotor {
private:
    uint8_t pin_pull_;
    uint8_t pin_dir_;
    uint8_t pin_ena_;
public:
    ArduinoMotor(uint8_t pin_pull, uint8_t pin_dir, uint8_t pin_ena):
        pin_pull_(pin_pull), pin_dir_(pin_dir), pin_ena_(pin_ena)
    {
        pinMode(pin_pull, OUTPUT);
        pinMode(pin_dir, OUTPUT);
        pinMode(pin_ena, OUTPUT);
    }

    bool pull(bool forward_direction) override { 
        if (forward_direction)
            digitalWrite(this->pin_dir_, HIGH);
        else
            digitalWrite(this->pin_dir_, LOW);
        
        digitalWrite(this->pin_pull_, HIGH);
        return true;
    }

    bool release() override {
        digitalWrite(this->pin_pull_, LOW);
        return true;
    }
};
#endif
