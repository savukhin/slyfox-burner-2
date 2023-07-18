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

    bool getRapidSpeed(bool isXAxis) { return (isXAxis ? cfg.rapid_speed_x_mm_s : cfg.rapid_speed_y_mm_s); }
    bool getSlowSpeed(bool isXAxis) { return (isXAxis ? cfg.slow_speed_x_mm_s : cfg.slow_speed_y_mm_s); }

    bool getAccel(bool isXAxis) { return (isXAxis ? cfg.accel_x_mm_s2 : cfg.accel_y_mm_s2); }
};
