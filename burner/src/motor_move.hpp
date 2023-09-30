#pragma once

#include <FreeRTOSConfig.h>

#include "connector.hpp"
#include "carriage.hpp"
#include "arduino_motor.hpp"
#include "my_crsf_serial.hpp"
#include "messages.hpp"
#include "config.hpp"
#include "arduino_serial.hpp"
#include "experiment.hpp"
#include "sensors.hpp"
#include "ignitor.hpp"

struct vMotorMoveType {
    long long req_id;
    motor_move_message_t* msg;
    TaskHandle_t xMotorMoveHandle;
    ICarriage* carriage_x;
    ICarriage* carriage_y;
    Config* config;
    IConnector *connector;
};

void vStartMotorMove(void *vParams) {
    auto params = (vMotorMoveType*)vParams;

    // Serial.printf("PRIMARY IN RTOS: rapid speed %d slow speed %d accel_x %f accel_y %f\n", 
    //     params->config->toMessage().rapid_speed_x_mm_s, params->config->toMessage().slow_speed_x_mm_s,
    //     params->config->toMessage().accel_x_mm_s2, params->config->toMessage().accel_y_mm_s2
    // );
    // Serial.flush();

    long long req_id = params->req_id;
    motor_move_message_t* msg = params->msg;
    TaskHandle_t xMotorMoveHandle = params->xMotorMoveHandle;

    // Serial.printf("Got req_id %d in task and config speed is %d and slow speed %d accel_x %f accel_x %f\n", 
    //     req_id, config.toMessage().rapid_speed_x_mm_s, config.toMessage().slow_speed_x_mm_s, 
    //     config.toMessage().accel_x_mm_s2, config.toMessage().accel_y_mm_s2);

    bool isXAxis = ((msg->misc & 0x01) == 0);
    ICarriage *carriage = params->carriage_x;
    if (!isXAxis) { 
        carriage = params->carriage_y;
    }

    double speed = params->config->getSlowSpeed(isXAxis);
    if (msg->misc & 0x02 == 1) {
        speed = params->config->getRapidSpeed(isXAxis);
    }

    double accel = params->config->getAccel(isXAxis);

    // printf("Move To speed %d accel: %f isXAsix %d\n", speed, accel, isXAxis);
    // Serial.printf("Move To speed %d accel: %f isXAsix %d\n", speed, accel, isXAxis);

    // Serial.printf("MOVE TO IN RTOS: rapid speed %d slow speed %d accel_x %f accel_y %f\n", 
    //     config.toMessage().rapid_speed_x_mm_s, config.toMessage().slow_speed_x_mm_s,
    //     config.toMessage().accel_x_mm_s2, config.toMessage().accel_y_mm_s2
    // );

    MoveResponse moved = carriage->moveTo(msg->position_mm, speed, accel);
    // printf("Made Move To\n");
    // auto resp_msg = new motor_move_message_t{moved.final_position_mm, msg->misc};
    // if (moved.interrupted) {
    //     resp_msg->misc = (resp_msg->misc | 0x04);
    // }
    // Serial.printf("Moved\n");
    // Serial.flush();
    auto resp_msg = new current_position_message_t{
        params->carriage_x->getCurrentPosition(),
        params->carriage_y->getCurrentPosition()
    };
    // Serial.printf("Created msg\n");
    // Serial.flush();

    CurrentPositionMessage response(resp_msg);
    // Serial.printf("Sending msg\n");
    // Serial.flush();
    // printf("Sending msg\n");
    params->connector->sendMessage(response, req_id);

    // Serial.printf("Msg sended, finishing xTask\n");
    // Serial.flush();

    vTaskDelete(xMotorMoveHandle);

    // delete vParams;
}
