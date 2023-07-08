#pragma once

#include "imessage.hpp"

typedef struct config_message_s {
    uint16_t rapid_speed_mm_s; // Rapid speed millimeter/second
    uint16_t slow_speed_mm_s; // Slow speed illimeter/second
} PACKED config_message_t;

class ConfigMessage : public IMessage {
private:
    config_message_t *msg;
public:
    uint8_t get_id() override { return 1; }
    void fromBytes(void *bytes) override { this->msg = (config_message_t*)bytes; }
    void* toBytes() override { return (void*)this->msg; }
};
