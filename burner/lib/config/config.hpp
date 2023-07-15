#pragma once

#include "messages.hpp"

class Config {
private:
    config_message_t cfg;

public:
    Config() = default;

    void fromMessage(const config_message_t &msg) { this->cfg = msg; }
    void fromMessage(const config_message_t *msg) { this->cfg = *msg; }

    config_message_t toMessage() { return cfg; }
};
