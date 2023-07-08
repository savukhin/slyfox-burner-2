#pragma once

#include "imessage.hpp"

class ConfigMessage : public IMessage {
public:
    uint8 get_id() override {
        return 1;
    }
}