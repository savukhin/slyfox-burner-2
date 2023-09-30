#pragma once

#define NC 0

#define USB_BAUDRATE 115200

#define PIN_IGNITOR  NC
#define PIN_PHOTO_RESISTOR  NC

#define PIN_X_PULL NC
#define PIN_X_DIR  NC
#define PIN_X_ENA  NC

#define PIN_Y_PULL NC
#define PIN_Y_DIR  NC
#define PIN_Y_ENA  NC

#define MOTOR_PULSE_MIN_TIME_MS 10

#define MOTOR_X_MAX_MM        300
#define MOTOR_X_PULSE_PER_MM  300

#define MOTOR_Y_MAX_MM        300
#define MOTOR_Y_PULSE_PER_MM  300

// #define REQ_TYPE long
// #define LEN_TYPE int
// #define MSG_TYPE uint8_t
#define REQ_TYPE uint64_t
#define LEN_TYPE uint16_t
#define MSG_TYPE uint8_t

// #define DEFAULT_RAPID_SPEED_X_MM_S 1000
// #define DEFAULT_RAPID_SPEED_Y_MM_S 1000
// #define DEFAULT_SLOW_SPEED_X_MM_S 1000
// #define DEFAULT_SLOW_SPEED_Y_MM_S 1000
#define DEFAULT_RAPID_SPEED_X_MM_S 5
#define DEFAULT_RAPID_SPEED_Y_MM_S 5
#define DEFAULT_SLOW_SPEED_X_MM_S 5
#define DEFAULT_SLOW_SPEED_Y_MM_S 5
#define DEFAULT_ACCEL_X 10
#define DEFAULT_ACCEL_Y 10
#define DEFAULT_X_MM 0
#define DEFAULT_Y1_MM 0
#define DEFAULT_Y2_MM 0
