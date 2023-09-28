#include <Arduino.h>
#include <FreeRTOSConfig.h>
#include <pinout.hpp>
#include <utility>
#include <vector>
#include <tuple>

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

#define RD_TLS __thread

IConnector *connector;
ICarriage *carriage_x;
ICarriage *carriage_y;
Config config;
IIgnitor *ignitor;
IExperiment *experiment;
std::vector<ISensor*> sensors = { new PhotoResistor(PIN_PHOTO_RESISTOR) };
// ISensor *(sensors[]) = { new PhotoResistor(PIN_PHOTO_RESISTOR) };

void vConnectorStart(void *vParams) {
    connector->start();
}

void vSensorReadAndSend(void *vParams) {
    for (;;) {
        sensors_t *msg = new sensors_t{};
        msg->values = new uint16_t[sensors.size()];

        for (int i = 0; i < sensors.size(); i++) {
            auto val = sensors[i]->read();
            msg->values[i] = val;
        }

        SensorsMessage<1> message(msg);
        connector->sendMessage(message, 0);
    }
}

struct vMotorMoveType {
    long long req_id;
    motor_move_message_t* msg;
    TaskHandle_t xMotorMoveHandle;
};

void vStartMotorMove(void *vParams) {
    // Serial.printf("PRIMARY IN RTOS: rapid speed %d slow speed %d accel_x %f accel_y %f\n", 
    //     config.toMessage().rapid_speed_x_mm_s, config.toMessage().slow_speed_x_mm_s,
    //     config.toMessage().accel_x_mm_s2, config.toMessage().accel_y_mm_s2
    // );

    auto params = (vMotorMoveType*)vParams;

    long long req_id = params->req_id;
    motor_move_message_t* msg = params->msg;
    TaskHandle_t xMotorMoveHandle = params->xMotorMoveHandle;

    // Serial.printf("Got req_id %d in task and config speed is %d and slow speed %d accel_x %f accel_x %f\n", 
    //     req_id, config.toMessage().rapid_speed_x_mm_s, config.toMessage().slow_speed_x_mm_s, 
    //     config.toMessage().accel_x_mm_s2, config.toMessage().accel_y_mm_s2);

    bool isXAxis = ((msg->misc & 0x01) == 0);
    ICarriage *carriage = carriage_x;
    if (!isXAxis) { 
        carriage = carriage_y;
    }

    double speed = config.getSlowSpeed(isXAxis);
    if (msg->misc & 0x02 == 1) {
        speed = config.getRapidSpeed(isXAxis);
    }

    double accel = config.getAccel(isXAxis);

    // Serial.printf("Move To speed %d accel: %f isXAsix %d\n", speed, accel, isXAxis);

    // Serial.printf("MOVE TO IN RTOS: rapid speed %d slow speed %d accel_x %f accel_y %f\n", 
    //     config.toMessage().rapid_speed_x_mm_s, config.toMessage().slow_speed_x_mm_s,
    //     config.toMessage().accel_x_mm_s2, config.toMessage().accel_y_mm_s2
    // );

    MoveResponse moved = carriage->moveTo(msg->position_mm, speed, accel);
    // auto resp_msg = new motor_move_message_t{moved.final_position_mm, msg->misc};
    // if (moved.interrupted) {
    //     resp_msg->misc = (resp_msg->misc | 0x04);
    // }
    // Serial.printf("Moved\n");
    // Serial.flush();
    auto resp_msg = new current_position_message_t{
        carriage_x->getCurrentPosition(),
        carriage_y->getCurrentPosition()
    };
    // Serial.printf("Created msg\n");
    // Serial.flush();

    CurrentPositionMessage response(resp_msg);
    // Serial.printf("Sending msg\n");
    // Serial.flush();
    connector->sendMessage(response, req_id);

    // Serial.printf("Msg sended, finishing xTask\n");
    // Serial.flush();

    vTaskDelete(xMotorMoveHandle);

    // delete vParams;
}

void setup() {
    IByteHandler *byte_handler = new MyCrsfSerial<REQ_TYPE, LEN_TYPE, MSG_TYPE>();
    ISerial *serial = new ArduinoSerial(USB_BAUDRATE);

    connector = new Connector(byte_handler, serial);


    // delay(5000);
    // Serial.println("start setup");


    IMotor *motor_x = new ArduinoMotor(PIN_X_PULL, PIN_X_DIR, PIN_X_ENA);
    IMotor *motor_y = new ArduinoMotor(PIN_Y_PULL, PIN_Y_DIR, PIN_Y_ENA);

    carriage_x = new Carriage(motor_x, 0, MOTOR_X_MAX_MM, MOTOR_X_PULSE_PER_MM);
    carriage_y = new Carriage(motor_y, 0, MOTOR_Y_MAX_MM, MOTOR_Y_PULSE_PER_MM);

    ignitor = new Ignitor(PIN_IGNITOR);

    experiment = new Experiment(carriage_x, carriage_y, ignitor);

    // Get config
    connector->subscribe(GetConfigMessage().get_id(), [](const void*, int req_id) {
        config_message_t *msg = new config_message_t;
        auto msg_cop = config.toMessage();
        memcpy(msg, &msg_cop, sizeof(config_message_t));
        ConfigMessage result(msg);
        connector->sendMessage(result, req_id);
        // delete msg;
    });

    // Set config
    connector->subscribe(ConfigMessage().get_id(), [](const void* received_msg, int req_id) {
        config_message_t *msg = (config_message_t*)received_msg;

        config.fromMessage(msg);
        // Serial.printf("Setting rapid speed in config %d from msg %d\n", config.toMessage().rapid_speed_x_mm_s, msg->rapid_speed_x_mm_s);
        connector->sendMessage(ResponseMessage(0), req_id);
        delete msg;

        // Serial.printf("Set rapid speed in config %d\n", config.toMessage().rapid_speed_x_mm_s);
    });

    connector->subscribe(InterruptMessage().get_id(), [](const void* received_msg, int req_id) {
        // Serial.printf("Got interrupt message\n");
        // if (experiment->is_started()) experiment->stop();
        carriage_x->stop();
        carriage_y->stop();

        connector->sendMessage(InterruptResponseMessage(0), req_id);
    });

    connector->subscribe(MotorMoveMessage().get_id(), [](const void* received_msg, int req_id) {
        // Serial.printf("Got motor move req\n");
        xTaskHandle xMotorMoveHandle = NULL;
        BaseType_t xReturned;

        auto msg = new vMotorMoveType;
        msg->req_id = req_id;
        msg->msg = (motor_move_message_t*)received_msg;
        msg->xMotorMoveHandle = xMotorMoveHandle;
        // Serial.printf("Not in rtos: rapid speed %d slow speed %d accel_x %f accel_y %f\n", 
        //     config.toMessage().rapid_speed_x_mm_s, config.toMessage().slow_speed_x_mm_s,
        //     config.toMessage().accel_x_mm_s2, config.toMessage().accel_y_mm_s2
        // );

        // msg->position_mm, speed, config.getAccel(isXAxis)
        xReturned = xTaskCreate(
                    vStartMotorMove,       /* Function that implements the task. */
                    "MOTOR_MOVE",                   /* Text name for the task. */
                    10000,                      /* Stack size in words, not bytes. */
                    msg,             /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,         /* Priority at which the task is created. */
                    &xMotorMoveHandle );               /* Used to pass out the created task's handle. */

        // if( xReturned == pdPASS )
        // {
        //     Serial.printf("Deleting xTask\n");
        //     Serial.flush();
        //     /* The task was created.  Use the task's handle to delete the task. */
        //     vTaskDelete( xHandle );
        // }
    });

    connector->subscribe(GetCurrentPositionMessage().get_id(), [](const void* received_msg, int req_id) {
        auto resp_msg = new current_position_message_t{
            carriage_x->getCurrentPosition(),
            carriage_y->getCurrentPosition()
        };
        
        CurrentPositionMessage response(resp_msg);
        connector->sendMessage(response, req_id);
    });

    connector->subscribe(StartExperimentMessage().get_id(), [](const void*, int req_id) {
        experiment->start(config);
        connector->sendMessage(ResponseMessage(0), req_id);
    });

    connector->subscribe(StopExperimentMessage().get_id(), [](const void*, int req_id) {
        bool stopped = experiment->stop();
        uint8_t status = stopped;
        connector->sendMessage(ResponseMessage(status), req_id);
    });

    // BaseType_t xReturned;
    // TaskHandle_t xHandle = NULL;
    // xReturned = xTaskCreate(
    //                 vSensorReadAndSend,       /* Function that implements the task. */
    //                 "NAME",                   /* Text name for the task. */
    //                 512,                      /* Stack size in words, not bytes. */
    //                 ( void * ) 1,             /* Parameter passed into the task. */
    //                 tskIDLE_PRIORITY,         /* Priority at which the task is created. */
    //                 &xHandle );               /* Used to pass out the created task's handle. */

    // TaskHandle_t xHandle2 = NULL;
    // xTaskCreate(
    //                 vConnectorStart,       /* Function that implements the task. */
    //                 "NAME",                   /* Text name for the task. */
    //                 51200,                      /* Stack size in words, not bytes. */
    //                 // 5120,                      /* Stack size in words, not bytes. */
    //                 ( void * ) 1,             /* Parameter passed into the task. */
    //                 tskIDLE_PRIORITY,         /* Priority at which the task is created. */
    //                 &xHandle2 );               /* Used to pass out the created task's handle. */
}

void loop() {
    // Serial.println("loop");
    // delay(1000);
    vConnectorStart(nullptr);
    // delay(100);
    // Serial.println("blink up");
    // delay(1000);
    // Serial.println("blink down");
    // delay(1000);
}
