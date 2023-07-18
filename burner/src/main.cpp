#include <Arduino.h>
#include <FreeRTOSConfig.h>
#include <pinout.hpp>
#include "connector.hpp"
#include "carriage.hpp"
#include "arduino_motor.hpp"
#include "my_crsf_serial.hpp"
#include "messages.hpp"
#include "config.hpp"
#include "arduino_serial.hpp"
#include "experiment.hpp"

IConnector *connector;
ICarriage *carriage_x;
ICarriage *carriage_y;
Config config;
IExperiment *experiment;

void setup() {
    IByteHandler *byte_handler = new MyCrsfSerial<REQ_TYPE, LEN_TYPE, MSG_TYPE>();
    ISerial *serial = new ArduinoSerial(USB_BAUDRATE);

    connector = new Connector(byte_handler, serial);

    IMotor *motor_x = new ArduinoMotor(PIN_X_PULL, PIN_X_DIR, PIN_X_ENA);
    IMotor *motor_y = new ArduinoMotor(PIN_Y_PULL, PIN_Y_DIR, PIN_Y_ENA);

    carriage_x = new Carriage(motor_x, 0, MOTOR_X_MAX_MM, MOTOR_X_PULSE_PER_MM);
    carriage_y = new Carriage(motor_y, 0, MOTOR_Y_MAX_MM, MOTOR_Y_PULSE_PER_MM);

    experiment = new Experiment(carriage_x, carriage_y);


    // Get config
    connector->subscribe(GetConfigMessage().get_id(), [](const void*, int req_id) {
        config_message_t *msg = new config_message_t(config.toMessage());
        ConfigMessage result(msg);
        connector->sendMessage(result, req_id);
    });

    // Set config
    connector->subscribe(ConfigMessage().get_id(), [](const void* received_msg, int req_id) {
        config_message_t *msg = (config_message_t*)received_msg;
        config.fromMessage(msg);
        connector->sendMessage(ResponseMessage(0), req_id);
        delete msg;
    });

    connector->subscribe(MotorMoveMessage().get_id(), [](const void* received_msg, int req_id) {
        motor_move_message_t *msg = (motor_move_message_t*)received_msg;
        bool isXAxis = ((msg->misc & 0x01) == 0);
        ICarriage *carriage = carriage_x;
        if (!isXAxis) { 
            carriage = carriage_y;
        }

        uint16_t speed = config.getSlowSpeed(isXAxis);
        if (msg->misc & 0x02 == 1) {
            speed = config.getRapidSpeed(isXAxis);
        }

        MoveResponse moved = carriage->moveTo(msg->position_mm, speed, config.getAccel(isXAxis));
        auto resp_msg = new motor_move_message_t{moved.final_position_mm, msg->misc};
        if (moved.interrupted) {
            resp_msg->misc = (resp_msg->misc | 0x04);
        }
        MotorMoveMessage response(resp_msg);
        connector->sendMessage(response, req_id);
        delete resp_msg;
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
}

void loop() {
    
}
