#pragma once

#include "imessage.hpp"
#include "../../include/pinout.hpp"

template<typename T, uint8_t ID>
class Message : public IMessage {
protected:
    T *msg_;
public:
    Message() { this->msg_ = new T; };
    Message(T *msg) {
        this->msg_ = msg;
    }
    uint8_t get_id() const override { return ID; }
    void fromBytes(void *bytes) override { this->msg_ = (T*)bytes; }
   void* toBytes() const override { return (void*)this->msg_; }
    long long getByteLen() const override { return sizeof(T); }

    ~Message() { delete this->msg_; };
};

typedef struct empty_message_s {
    uint8_t placeholder=0;
} PACKED empty_message_t;

typedef struct response_message_s {
    uint8_t status_code;
} PACKED response_message_t;

typedef struct config_message_s {
    uint16_t rapid_speed_x_mm_s = DEFAULT_RAPID_SPEED_X_MM_S; // Rapid speed X-axis millimeter/second
    uint16_t slow_speed_x_mm_s = DEFAULT_SLOW_SPEED_X_MM_S; // Slow speed X-axis millimeter/second
    uint16_t rapid_speed_y_mm_s = DEFAULT_RAPID_SPEED_Y_MM_S; // Rapid speed Y-axis millimeter/second
    uint16_t slow_speed_y_mm_s = DEFAULT_SLOW_SPEED_Y_MM_S; // Slow speed Y-axis millimeter/second
    // -- 8 bytes -- //

    uint16_t accel_x_mm_s2 = DEFAULT_ACCEL_X; // Acceleration X-axis millimeter/(second^2)
    uint16_t accel_y_mm_s2 = DEFAULT_ACCEL_Y; // Acceleration Y-axis millimeter/(second^2)
    // -- 12 bytes -- //

    uint16_t x_mm = DEFAULT_X_MM; // Position in X-axis
    uint16_t y1_mm = DEFAULT_Y1_MM; // First position in Y-axis
    uint16_t y2_mm = DEFAULT_Y2_MM; // Second position in Y-axis
    // -- 18 bytes -- //
} PACKED config_message_t;

typedef struct motor_move_message_s {
    double position_mm;
    uint8_t misc; 
    // misc & 0x01 = axis (0 = X, 1 = Y)
    // misc & 0x02 = speed (0 = slow, 1 - rapid); 
    // misc & 0x04 = interrupt (1 = true, 0 = false)
} PACKED motor_move_message_t;

typedef struct current_position_message_s {
    double x;
    double y;
} PACKED current_position_message_t;

typedef struct start_experiment_s {
} PACKED start_experiment_t;

typedef struct get_experiment_state_s {
    uint8_t is_started;
} PACKED get_experiment_state_t;

typedef struct sensors_s {
    uint16_t *values;
} PACKED sensors_t;

template <uint8_t ID, uint8_t Len>
class SensorsMessage_ : public IMessage {
private:
    sensors_t *msg_;
public:
    SensorsMessage_() {}
    SensorsMessage_(sensors_t *msg) : msg_(msg) { }
    uint8_t get_id() const override { return ID; }
    void fromBytes(void *bytes) override { 
        this->msg_ = new sensors_t;
        this->msg_->values = new uint16_t[Len];
        uint16_t *translated = (uint16_t*)bytes;

        for (int i = 0; i < Len; i++) {
            this->msg_->values[i] = translated[i];
        }
    }
    void* toBytes() const override { 
        uint8_t *result = new uint8_t[Len];
        for (int i = 0; i < Len; i++) {
            result[i] = this->msg_->values[i];
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

    ~ResponseMessage_()  = default;
};

using EmptyMessage = Message<empty_message_t, 1>;
using ResponseMessage = ResponseMessage_<2>;
using ConfigMessage = Message<config_message_t, 3>;
using GetConfigMessage = Message<empty_message_t, 4>;
using MotorMoveMessage = Message<motor_move_message_t, 5>;
using GetCurrentPositionMessage = Message<empty_message_t, 6>;
using CurrentPositionMessage = Message<current_position_message_t, 7>;
template<uint8_t Len>
using SensorsMessage = SensorsMessage_<8, Len>;
using StartExperimentMessage = Message<start_experiment_t, 9>;
using GetExperimentStateMessage = Message<get_experiment_state_t, 10>;
using StopExperimentMessage = Message<empty_message_t, 11>;
