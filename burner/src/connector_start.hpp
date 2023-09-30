#pragma once

// #include <Arduino.h>
#include <FreeRTOSConfig.h>

#include "iconnector.hpp"
#include "carriage.hpp"
#include "arduino_motor.hpp"
#include "my_crsf_serial.hpp"
#include "messages.hpp"
#include "config.hpp"
#include "arduino_serial.hpp"
#include "experiment.hpp"
#include "sensors.hpp"
#include "ignitor.hpp"

#include "motor_move.hpp"


struct vConnectorStruct {
    IConnector *connector;
    Config *config;
    ICarriage* carriage_x;
    ICarriage* carriage_y;
    IExperiment* experiment;
};

void vConnectorStart(void *vParams) {
    vConnectorStruct* params = (vConnectorStruct*)vParams;

    // Get config
    params->connector->subscribe(GetConfigMessage().get_id(), [&](const void*, int req_id) {
        // Serial.println("Entered callback");
        // Serial.flush();
        // printf("Entered callback but printf");
        config_message_t *msg = new config_message_t;
        // Serial.println("Calling toMessage");
        auto msg_cop = params->config->toMessage();
        // Serial.printf("memcpy; config accel_x=%f\n", msg_cop.accel_x_mm_s2);
        memcpy(msg, &msg_cop, sizeof(config_message_t));
        // Serial.println("makig result");
        ConfigMessage result(msg);
        // Serial.printf("sending to is NULL %d\n", (params->connector == nullptr));
        // params->connector->
        params->connector->sendMessage(result, req_id);
        // delete msg;
    });

    // Set config
    params->connector->subscribe(ConfigMessage().get_id(), [&](const void* received_msg, int req_id) {
        config_message_t *msg = (config_message_t*)received_msg;

        params->config->fromMessage(msg);
        // Serial.printf("Setting rapid speed in config %d from msg %d\n", config->toMessage().rapid_speed_x_mm_s, msg->rapid_speed_x_mm_s);
        params->connector->sendMessage(ResponseMessage(0), req_id);
        delete msg;

        // Serial.printf("Set rapid speed in config %d\n", config->toMessage().rapid_speed_x_mm_s);
    });

    params->connector->subscribe(InterruptMessage().get_id(), [&](const void* received_msg, int req_id) {
        // Serial.printf("Got interrupt message\n");
        // if (experiment->is_started()) experiment->stop();
        params->carriage_x->stop();
        params->carriage_y->stop();

        params->connector->sendMessage(InterruptResponseMessage(0), req_id);
    });

    params->connector->subscribe(MotorMoveMessage().get_id(), [&](const void* received_msg, int req_id) {
        // Serial.printf("Got motor move req\n");
        // printf("Got motor move req but in plain printf\n");
        xTaskHandle xMotorMoveHandle = NULL;
        BaseType_t xReturned;

        auto msg = new vMotorMoveType{
            req_id,
            (motor_move_message_t*)received_msg,
            xMotorMoveHandle,
            params->carriage_x,
            params->carriage_y,
            params->config,
            params->connector
        };
        
        // Serial.printf("Not in rtos: rapid speed %d slow speed %d accel_x %f accel_y %f\n", 
        //     params->config->toMessage().rapid_speed_x_mm_s, params->config->toMessage().slow_speed_x_mm_s,
        //     params->config->toMessage().accel_x_mm_s2, params->config->toMessage().accel_y_mm_s2
        // );
        // Serial.flush();

        // msg->position_mm, speed, config->getAccel(isXAxis)
        xReturned = xTaskCreate(
                    vStartMotorMove,       /* Function that implements the task. */
                    "MOTOR_MOVE",                   /* Text name for the task. */
                    5 * 10000,                      /* Stack size in words, not bytes. */
                    msg,             /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,         /* Priority at which the task is created. */
                    &xMotorMoveHandle );               /* Used to pass out the created task's handle. */

        if( xReturned == pdPASS )
        {
            Serial.printf("xTask is pdPASS\n");
            Serial.flush();
            /* The task was created.  Use the task's handle to delete the task. */
            // vTaskDelete( xHandle );
        } else {
            Serial.printf("xTask NOT pdPASS but %d\n", xReturned);
            Serial.flush();
        }
    });

    params->connector->subscribe(GetCurrentPositionMessage().get_id(), [&](const void* received_msg, int req_id) {
        auto resp_msg = new current_position_message_t{
            params->carriage_x->getCurrentPosition(),
            params->carriage_y->getCurrentPosition()
        };
        
        CurrentPositionMessage response(resp_msg);
        params->connector->sendMessage(response, req_id);
    });

    params->connector->subscribe(StartExperimentMessage().get_id(), [&](const void*, int req_id) {
        params->experiment->start(params->config);
        params->connector->sendMessage(ResponseMessage(0), req_id);
    });

    params->connector->subscribe(StopExperimentMessage().get_id(), [&](const void*, int req_id) {
        bool stopped = params->experiment->stop();
        uint8_t status = stopped;
        params->connector->sendMessage(ResponseMessage(status), req_id);
    });

    params->connector->start();

    vTaskDelete(NULL);
}