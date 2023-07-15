#pragma once

#include "imessage.hpp"

template<typename T, uint8_t ID>
class Message : public IMessage {
private:
    T *msg_;
public:
    Message() = default;
    Message(T *msg) { this->msg_ = msg; }
    uint8_t get_id() override { return ID; }
    void fromBytes(void *bytes) override { this->msg_ = (T*)bytes; }
    void* toBytes() override { return (void*)this->msg_; }
};

typedef struct empty_message_s {
} PACKED empty_message_t;

typedef struct response_message_s {
    uint8_t status_code;
} PACKED response_message_t;

typedef struct config_message_s {
    uint16_t rapid_speed_x_mm_s; // Rapid speed X-axis millimeter/second
    uint16_t slow_speed_x_mm_s; // Slow speed X-axis millimeter/second
    uint16_t rapid_speed_y_mm_s; // Rapid speed Y-axis millimeter/second
    uint16_t slow_speed_y_mm_s; // Slow speed Y-axis millimeter/second
    // -- 8 bytes -- //

    uint16_t accel_speed_x_mm_s2; // Acceleration X-axis millimeter/(second^2)
    uint16_t accel_speed_y_mm_s2; // Acceleration Y-axis millimeter/(second^2)
    // -- 12 bytes -- //

    uint16_t y_mm; // Position in Y-axis
    uint16_t x1_mm; // First position in X-axis
    uint16_t x2_mm; // Second position in X-axis
    // -- 16 bytes -- //
} PACKED config_message_t;

typedef struct motor_move_message_s {
    uint16_t position_mm;
} PACKED motor_move_message_t;

typedef struct start_experiment_s {
} PACKED start_experiment_t;

typedef struct get_experiment_state_s {
    uint8_t is_started;
} PACKED get_experiment_state_t;

typedef struct sensors_s {
    uint8_t *values;
} PACKED sensors_t;

template <uint8_t ID, uint8_t Len>
class SensorsMessage_ : public IMessage {
private:
    sensors_t *msg;
public:
    uint8_t get_id() override { return ID; }
    void fromBytes(void *bytes) override { 
        this->msg = new sensors_t;
        this->msg->values = new uint8_t[Len];
        uint8_t *translated = (uint8_t*)bytes;

        for (int i = 0; i < Len; i++) {
            this->msg->values[i] = translated[i];
        }
    }
    void* toBytes() override { 
        uint8_t *result = new uint8_t[Len];
        for (int i = 0; i < Len; i++) {
            result[i] = this->msg->values[i];
        }
        return result;
    }
};

using ResponseMessage = Message<response_message_t, 1>;
using ConfigMessage = Message<config_message_t, 2>;
using GetConfigMessage = Message<empty_message_t, 3>;
using MotorMoveMessage = Message<motor_move_message_t, 4>;
template<uint8_t Len>
using SensorsMessage = SensorsMessage_<5, Len>;
using StartExperimentMessage = Message<start_experiment_t, 6>;
using GetExperimentStateMessage = Message<get_experiment_state_t, 7>;
