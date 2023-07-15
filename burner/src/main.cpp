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

Connector *connector;
ICarriage *carriage_x;
ICarriage *carriage_y;
Config config;

void setup() {
    IByteHandler *byte_handler = new MyCrsfSerial<long, int, uint8_t>();
    ISerial *serial = new ArduinoSerial(USB_BAUDRATE);

    connector = new Connector(byte_handler, serial);

    IMotor *motor_x = new ArduinoMotor(PIN_X_PULL, PIN_X_DIR, PIN_X_ENA);
    IMotor *motor_y = new ArduinoMotor(PIN_Y_PULL, PIN_Y_DIR, PIN_Y_ENA);

    carriage_x = new Carriage(motor_x, 0, MOTOR_X_MAX_MM, MOTOR_X_PULSE_PER_MM);
    carriage_y = new Carriage(motor_y, 0, MOTOR_Y_MAX_MM, MOTOR_Y_PULSE_PER_MM);

    connector->subscribe(GetConfigMessage().get_id(), [](void*, int) {
        config_message_t *msg = new config_message_t(config.toMessage());
        auto result = ConfigMessage(msg);
        connector->sendMessage(result);
    });
    connector->subscribe(ConfigMessage().get_id(), [](void*, int) {
        
    });

    connector->subscribe(MotorMoveMessage().get_id(), [](void*, int) {
        
    });

    connector->subscribe(StartExperimentMessage().get_id(), [](void*, int) {
        
    });

    connector->subscribe(GetExperimentStateMessage().get_id(), [](void*, int) {
        
    });
}

void loop() {
    
}
