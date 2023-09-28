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
    std::atomic<float> current_position_mm_ = { 0 };
    double pulse_per_mm_;
    double mm_per_pulse_;


    std::atomic_bool busy_ = { false };

    std::atomic_bool paused_ = { false };
    std::atomic_bool need_stop_ = { false };

    unsigned long calculate_count_of_pulls(float distance_mm) {
        return distance_mm * pulse_per_mm_;
    }

    static TickType_t toTick(const double delay_ms) {
        return delay_ms / portTICK_PERIOD_MS;
    }

    TickType_t motor_pull_ticks = this->toTick(MOTOR_PULSE_MIN_TIME_MS);

public:
    Carriage(IMotor *motor, float min_position_mm, float max_position_mm, int pulse_per_mm, float current_position_mm=0) : 
        motor_(motor), min_position_mm_(min_position_mm), max_position_mm_(max_position_mm), pulse_per_mm_(pulse_per_mm), mm_per_pulse_(1. / pulse_per_mm), current_position_mm_(current_position_mm) {}
    
    // MoveResponse moveTo(float position_mm, float speed_mm_s, float accell_mm_s2) override {
    //     if (!this->motor_) {
    //         throw new std::runtime_error("no motor defined");
    //     }

    //     this->busy_ = true;

    //     float desired_position_mm = position_mm;
    //     if (position_mm > this->max_position_mm_) {
    //         desired_position_mm = this->max_position_mm_;
    //     } else if (position_mm < this->min_position_mm_) {
    //         desired_position_mm = this->min_position_mm_;
    //     }

    //     float distance = desired_position_mm - current_position_mm_;
    //     bool is_forward = (distance > 0);
    //     auto pulls = this->calculate_count_of_pulls(distance);

    //     auto start_time = millis();
    //     double deltaS_mm = 1. / this->pulse_per_mm_;
    //     double koef = 2. * accell_mm_s2 * deltaS_mm;
        
    //     auto max_delta_time = speed_mm_s / accell_mm_s2;
    //     // auto min_delta_time = speed_mm_s / accell_mm_s2;
    //     auto max_delay_ms_half = 1000 * (deltaS_mm / (speed_mm_s * 2));
    //     auto max_delay_ms_half_ticks = this->toTick(max_delay_ms_half);

    //     auto last_frame_speed_mm_s = 0;
    //     unsigned long prev_time_desired_ms = 0;

    //     float final_position_mm = this->current_position_mm_;

    //     bool interrupted = false;

    //     // Serial.printf("start_time=%d  deltaS_mm=%f  koef=%f  max_delta_time=%f  max_delay_ms_half=%f   max_delay_ms_half_ticks=%f\n",
    //     //         start_time, deltaS_mm, koef, max_delta_time, max_delay_ms_half, max_delay_ms_half_ticks);
                

    //     // Serial.printf("pulls count %d\n", pulls);

    //     for (unsigned long i = 0; i < pulls; i++) {
    //         // Serial.printf("i =%d\n", i);
    //         if (this->need_stop_) {
    //             // Serial.printf("need stop in carriage\n");
    //             this->need_stop_ = false;
    //             interrupted = true;
    //             break;
    //         }
    //         if (this->paused_) {
    //             i--;
    //             continue;
    //         }

    //         this->motor_->pull(is_forward);
    //         this->current_position_mm_ = this->current_position_mm_ + this->mm_per_pulse_;

    //         auto deltaT = millis() - start_time;
    //         if (deltaT < max_delta_time) {
    //             // Serial.println("first delay plato");
    //             vTaskDelay(max_delay_ms_half_ticks);
    //             this->motor_->release();
    //             // Serial.println("ssecond delay plato");
    //             vTaskDelay(max_delay_ms_half_ticks);
    //             // final_position_mm = current_position_mm_ + deltaT * speed_mm_s;

    //             continue;
    //         }

    //         double current_speed_mm_s = accell_mm_s2 * deltaT;

    //         // double v1 = last_frame_speed_mm_s;
    //         // double delay_s = (-v1 - sqrt(v1 * v1 + koef)) / (2. * accell_mm_s2);
    //         // double delay_ms = delay_s * 1000.;
    //         // // double delay_s_half = 1000 * (delay_s / 2);
    //         // double delay_ms_half = 1000. * (delay_ms / 2.);
    //         // double delay_ms_half_ticks = this->toTick(delay_ms_half);

    //         // Serial.printf("current_speed_mm_s=%f  v1=%f  delay_s=%f  delay_ms=%f  delay_ms_half=%f   delay_ms_half_ticks=%f\n",
    //         //     current_speed_mm_s, v1, delay_s, delay_ms, delay_ms_half, delay_ms_half_ticks);

    //         double time_desired_s = ((2. * (i + 1) * this->mm_per_pulse_) / accell_mm_s2);
    //         double time_desired_ms = time_desired_s * 1000.;

    //         double delay_ms = time_desired_ms - prev_time_desired_ms;
    //         double delay_ms_half = delay_ms / 2.;
    //         double delay_ms_half_ticks = this->toTick(delay_ms_half);

    //         // Serial.printf("mm_per_pulse_=%f, accell_mm_s2=%f, prev_time_desired_ms=%d, current_speed_mm_s=%f  time_desired_s=%f  time_desired_ms=%f  delay_ms=%f  delay_ms_half=%f   delay_ms_half_ticks=%f\n",
    //         //     this->mm_per_pulse_, accell_mm_s2, prev_time_desired_ms, current_speed_mm_s, time_desired_s, time_desired_ms, delay_ms, delay_ms_half, delay_ms_half_ticks);
            
                
    //         // Serial.printf("first delay while acceleration %f ticks %d\n", delay_ms_half, delay_ms_half_ticks);
    //         vTaskDelay(delay_ms_half_ticks);
    //         this->motor_->release();
    //         // Serial.println("second delay while acceleration");
    //         vTaskDelay(delay_ms_half_ticks);
    //         // final_position_mm = current_position_mm_ + deltaT * speed_mm_s;

    //         last_frame_speed_mm_s = current_speed_mm_s;
    //         prev_time_desired_ms = time_desired_ms;
    //     }

    //     // Serial.println("Finished pulling");

    //     this->busy_ = false;

    //     // Serial.println("Returning");
    //     return MoveResponse{this->current_position_mm_, interrupted};
    // }

    MoveResponse moveTo(float position_mm, float speed_mm_s, float accell_mm_s2) override {
        if (!this->motor_) {
            throw new std::runtime_error("no motor defined");
        }

        this->busy_ = true;

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
        unsigned long move_time_ms = 0;
        unsigned long displacement_mm = 0;

        double accell_mm_ms2 = accell_mm_s2 / 1e6;

        double deltaS_mm = 1. / this->pulse_per_mm_;
        double koef = 2. * accell_mm_s2 * deltaS_mm;

        
        auto max_delta_time = speed_mm_s / accell_mm_s2;
        // auto min_delta_time = speed_mm_s / accell_mm_s2;
        auto max_delay_ms_half = 1000 * (deltaS_mm / (speed_mm_s * 2));
        auto max_delay_us_half = 1000 * max_delay_ms_half;
        auto max_delay_ms_half_ticks = this->toTick(max_delay_ms_half);

        auto last_frame_speed_mm_s = 0;
        unsigned long prev_time_desired_ms = 0;

        float final_position_mm = this->current_position_mm_;

        bool interrupted = false;

        // Serial.printf("start_time=%d speed_mm_s=%f deltaS_mm=%f  koef=%f  max_delta_time=%f  max_delay_ms_half=%f max_delay_us_half=%f  max_delay_ms_half_ticks=%f portTICK_PERIOD_MS=%d\n",
        //         start_time, speed_mm_s, deltaS_mm, koef, max_delta_time, max_delay_ms_half, max_delay_us_half, max_delay_ms_half_ticks, portTICK_PERIOD_MS);
                

        // Serial.printf("pulls count %d\n", pulls);

        for (unsigned long i = 0; i < pulls; i++) {
            // Serial.printf("i =%d\n", i);
            if (this->need_stop_) {
                // Serial.printf("need stop in carriage\n");
                this->need_stop_ = false;
                interrupted = true;
                break;
            }
            if (this->paused_) {
                i--;
                continue;
            }
            auto tick_start_time = millis();



            if (true) {
            // if (move_time_ms < max_delta_time) {
                this->current_position_mm_ = this->current_position_mm_ + this->mm_per_pulse_;
                this->motor_->pull(is_forward);
                // Serial.println("first delay plato");
                // vTaskDelay(max_delay_ms_half_ticks);
                delayMicroseconds(max_delay_us_half);
                this->motor_->release();
                delayMicroseconds(max_delay_us_half);
                // Serial.println("ssecond delay plato");
                // vTaskDelay(max_delay_ms_half_ticks);
                // final_position_mm = current_position_mm_ + deltaT * speed_mm_s;
                
                move_time_ms += (millis() - tick_start_time);

                continue;
            }


            unsigned long current_time_ms = move_time_ms;
            unsigned long desired_time_s = sqrt((2 * displacement_mm) / accell_mm_s2);
            unsigned long desired_time_ms = desired_time_s * 1000;

            if (current_time_ms > desired_time_ms) {
                auto delta = desired_time_ms - current_time_ms;
                auto ticks = this->toTick(delta);
                vTaskDelay(ticks);
            } else {
                this->current_position_mm_ = this->current_position_mm_ + this->mm_per_pulse_;

                this->motor_->pull(is_forward);
                vTaskDelay(this->motor_pull_ticks);
                this->motor_->release();
                vTaskDelay(this->motor_pull_ticks);
            }

            unsigned long tick_time = millis() - tick_start_time;
            move_time_ms += tick_time;
        }

        // Serial.println("Finished pulling");

        this->busy_ = false;

        // Serial.println("Returning");
        return MoveResponse{this->current_position_mm_, interrupted};
    }

    MoveResponse moveLength(float distance_mm, float speed_mm_s, float accell_mm_s2) override {
        return this->moveTo(this->current_position_mm_ + distance_mm, speed_mm_s, accell_mm_s2);
    }

    float getCurrentPosition() override { return this->current_position_mm_; }
    
    bool isBusy() override {
        return this->busy_;
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
        // Serial.println("Called stop() carriage");
        if (this->isBusy()) {
            // Serial.println("need stop = true");
            this->need_stop_ = true;
            return true;
        }
        return false;
    }
};