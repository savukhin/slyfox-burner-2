#pragma once

#include <atomic>

#include "component_interfaces.hpp"
#include "config.hpp"
#include "experiment_interface.hpp"

/*
   |   (0,0)  ------------------> X-axis        |
   |   *-----------------------------           |
   |   |                            |           |
   |   |                            |           |
   |   |                            |           | <-- y1
   |   |                            |           |
   |   |                            |           |
   |   |                            |           |
   |   |                            |           |
   |   |  -------->===<------------ |           | <-- y2
   |   |                            |           |
   |   |                            |           |
   |   |                            |           |
   |   ------------------------------           |
   |                ^                           |
   |               | |                          Y
   |               | |                          
   Y               | |                         Y-axis
                   | |
Y-axis            -----
*/

class Experiment : public IExperiment {
private:
    std::atomic_bool started_;
    ICarriage *carriage_x_;
    ICarriage *carriage_y_;

public:
    Experiment(ICarriage *carriage_x, ICarriage *carriage_y): carriage_x_(carriage_x), carriage_y_(carriage_y) {}

    bool start(const Config &config) override {
        config_message_t cfg = config.toMessage();
        
        MoveResponse res;
        res = carriage_x_->moveTo(0, cfg.rapid_speed_x_mm_s, cfg.accel_x_mm_s2);
        if (res.interrupted) {
            return false;        
        }

        res = carriage_y_->moveTo(0, cfg.rapid_speed_y_mm_s, cfg.accel_y_mm_s2);
        if (res.interrupted) {
            return false;        
        }

        res = carriage_x_->moveTo(cfg.x_mm, cfg.rapid_speed_x_mm_s, cfg.accel_x_mm_s2);
        if (res.interrupted) {
            return false;        
        }
        res = carriage_x_->moveTo(cfg.y1_mm, cfg.rapid_speed_y_mm_s, cfg.accel_y_mm_s2);
        if (res.interrupted) {
            return false;        
        }

        res = carriage_x_->moveTo(cfg.y2_mm, cfg.rapid_speed_y_mm_s, cfg.accel_y_mm_s2);
        if (res.interrupted) {
            return false;        
        }
    }

    bool stop() override {
        return (carriage_y_->stop() || carriage_x_->stop());
    }
};
