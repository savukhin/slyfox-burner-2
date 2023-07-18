#pragma once

#include "imessage.hpp"

template<typename T, uint8_t ID>
class Message : public IMessage {
protected:
    T *msg_;
public:
    Message() = default;
    Message(T *msg) { this->msg_ = msg; }
    uint8_t get_id() const override { return ID; }
    void fromBytes(void *bytes) override { this->msg_ = (T*)bytes; }
    void* toBytes() const override { return (void*)this->msg_; }
    long long getByteLen() const override { return sizeof(T); }
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

    uint16_t accel_x_mm_s2; // Acceleration X-axis millimeter/(second^2)
    uint16_t accel_y_mm_s2; // Acceleration Y-axis millimeter/(second^2)
    // -- 12 bytes -- //

    uint16_t x_mm; // Position in X-axis
    uint16_t y1_mm; // First position in Y-axis
    uint16_t y2_mm; // Second position in Y-axis
    // -- 16 bytes -- //
} PACKED config_message_t;

typedef struct motor_move_message_s {
    float position_mm;
    uint8_t misc; 
    // misc & 0x01 = axis (0 = X, 1 = Y)
    // misc & 0x02 = speed (0 = slow, 1 - rapid); 
    // misc & 0x04 = interrupt (1 = true, 0 = false)
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
    long long getByteLen() const override { return Len; }
};

template <uint8_t ID>
class ResponseMessage_ : public Message<response_message_t, ID> {
public:
    ResponseMessage_() = default;
    ResponseMessage_(uint8_t status_code) {
        response_message_t *msg = new response_message_t{status_code};
        this->msg_ = msg;
    };
};

using EmptyMessage = Message<empty_message_t, 1>;
using ResponseMessage = ResponseMessage_<2>;
using ConfigMessage = Message<config_message_t, 3>;
using GetConfigMessage = Message<empty_message_t, 4>;
using MotorMoveMessage = Message<motor_move_message_t, 5>;
template<uint8_t Len>
using SensorsMessage = SensorsMessage_<6, Len>;
using StartExperimentMessage = Message<start_experiment_t, 7>;
using GetExperimentStateMessage = Message<get_experiment_state_t, 8>;
using StopExperimentMessage = Message<empty_message_t, 9>;
