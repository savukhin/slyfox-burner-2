#pragma once

#include "messages.hpp"

class Config {
private:
    config_message_t cfg;

public:
    Config() = default;

    void fromMessage(const config_message_t &msg) { this->cfg = msg; }
    void fromMessage(const config_message_t *msg) { this->cfg = *msg; }

    config_message_t toMessage() const { return cfg; }

    double getRapidSpeed(bool isXAxis) { return (isXAxis ? cfg.rapid_speed_x_mm_s : cfg.rapid_speed_y_mm_s); }
    double getSlowSpeed(bool isXAxis) { return (isXAxis ? cfg.slow_speed_x_mm_s : cfg.slow_speed_y_mm_s); }

    double getAccel(bool isXAxis) { return (isXAxis ? cfg.accel_x_mm_s2 : cfg.accel_y_mm_s2); }
    bool getX() { return cfg.x_mm; }
    bool getY1() { return cfg.y1_mm; }
    bool getY2() { return cfg.y2_mm; }
};
