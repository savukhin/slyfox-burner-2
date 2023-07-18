#pragma once

#include "component_interfaces.hpp"

#include <stdexcept>
#include <mutex>
#include <atomic>
#include <FreeRTOSConfig.h>
#include <math.h>
#include <utility>

class Carriage : public ICarriage {
private:
    IMotor *motor_;

    float min_position_mm_;
    float max_position_mm_;
    float current_position_mm_;
    int pulse_per_mm_;
    int mm_per_pulse_;

    std::mutex busy_mutex_;
    bool busy_;

    std::atomic_bool paused_;
    std::atomic_bool need_stop_;

    unsigned long calculate_count_of_pulls(float distance_mm) {
        return distance_mm * pulse_per_mm_;
    }

public:
    Carriage(IMotor *motor, float min_position_mm, float max_position_mm, int pulse_per_mm, float current_position_mm=0) : 
        motor_(motor), min_position_mm_(min_position_mm), max_position_mm_(max_position_mm), pulse_per_mm_(pulse_per_mm), mm_per_pulse_(1. / pulse_per_mm), current_position_mm_(current_position_mm) {}
    
    MoveResponse moveTo(float position_mm, float speed_mm_s, float accell_mm_s2) override {
        if (!this->motor_) {
            throw new std::runtime_error("no motor defined");
        }

        this->busy_mutex_.lock();
        this->busy_ = true;
        this->busy_mutex_.unlock();

        float desired_position_mm = position_mm;
        if (position_mm > this->max_position_mm_) {
            desired_position_mm = this->max_position_mm_;
        } else if (position_mm < this->min_position_mm_) {
            desired_position_mm = this->min_position_mm_;
        }

        float distance = desired_position_mm - current_position_mm_;
        bool is_forward = (distance > 0);
        auto pulls = this->calculate_count_of_pulls(distance);

        auto start_time = millis();
        float deltaS_mm = 1 / this->pulse_per_mm_;
        float koef = 2 * accell_mm_s2 * deltaS_mm;
        
        auto max_delta_time = speed_mm_s / accell_mm_s2;
        auto max_delay_ms_half = 1000 * (deltaS_mm / (speed_mm_s * 2));
        auto last_frame_speed_mm_s = 0;

        float final_position_mm = this->current_position_mm_;

        bool interrupted = false;

        for (unsigned long i = 0; i < pulls; i++) {
            if (this->need_stop_) {
                this->need_stop_ = false;
                interrupted = true;
                break;
            }
            if (this->paused_) {
                i--;
                continue;
            }

            this->motor_->pull(is_forward);
            this->current_position_mm_ += this->mm_per_pulse_;

            auto deltaT = millis() - start_time;
            if (deltaT > max_delta_time) {
                vTaskDelay(max_delay_ms_half);
                this->motor_->release();
                vTaskDelay(max_delay_ms_half);
                // final_position_mm = current_position_mm_ + deltaT * speed_mm_s;

                continue;
            }

            auto current_speed_mm_s = accell_mm_s2 * deltaT;

            auto v1 = last_frame_speed_mm_s;
            double delay_s = (-v1 - sqrt(v1 * v1 + koef)) / (2. * accell_mm_s2);
            double delay_s_half = 1000 * (delay_s / 2);

            vTaskDelay(delay_s_half);
            this->motor_->release();
            vTaskDelay(delay_s_half);
            // final_position_mm = current_position_mm_ + deltaT * speed_mm_s;

            last_frame_speed_mm_s = current_speed_mm_s;
        }

        this->busy_mutex_.lock();
        this->busy_ = false;
        this->busy_mutex_.unlock();

        return MoveResponse{this->current_position_mm_, interrupted};
    }

    MoveResponse moveLength(float distance_mm, float speed_mm_s, float accell_mm_s2) override {
        return this->moveTo(this->current_position_mm_ + distance_mm, speed_mm_s, accell_mm_s2);
    }

    float getCurrentPosition() override { return this->current_position_mm_; }
    
    bool isBusy() override {
        bool result;
        this->busy_mutex_.lock();
        result = this->busy_;
        this->busy_mutex_.unlock();
        return result;
    }

    bool pause() {
        if (this->isBusy()) {
            this->paused_ = true;
            return true;
        }
        return false;
    }

    bool resume() {
        if (this->isBusy()) {
            this->paused_ = false;
            return true;
        }
        return false;
    }

    bool stop() {
        if (this->isBusy()) {
            this->need_stop_ = true;
            return true;
        }
        return false;
    }
};