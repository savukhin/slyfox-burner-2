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

#include "connector_start.hpp"
#include "motor_move.hpp"

#define RD_TLS __thread

// IConnector *connector;
// ICarriage *carriage_x;
// ICarriage *carriage_y;
// Config config;
// IIgnitor *ignitor;
// IExperiment *experiment;
// std::vector<ISensor*> sensors = { new PhotoResistor(PIN_PHOTO_RESISTOR) };
// ISensor *(sensors[]) = { new PhotoResistor(PIN_PHOTO_RESISTOR) };



// void vSensorReadAndSend(void *vParams) {
//     for (;;) {
//         sensors_t *msg = new sensors_t{};
//         msg->values = new uint16_t[sensors.size()];

//         for (int i = 0; i < sensors.size(); i++) {
//             auto val = sensors[i]->read();
//             msg->values[i] = val;
//         }

//         SensorsMessage<1> message(msg);
//         connector->sendMessage(message, 0);
//     }
// }

void setup() {
    Config *config = new Config;

    IByteHandler *byte_handler = new MyCrsfSerial<REQ_TYPE, LEN_TYPE, MSG_TYPE>();

    HardwareSerial *hardware_serial = new HardwareSerial(Serial);

    ISerial *serial = new ArduinoSerial(USB_BAUDRATE, hardware_serial);

    IConnector *connector = new Connector(byte_handler, serial);

    // delay(5000);
    // Serial.println("start setup");


    IMotor *motor_x = new ArduinoMotor(PIN_X_PULL, PIN_X_DIR, PIN_X_ENA);
    IMotor *motor_y = new ArduinoMotor(PIN_Y_PULL, PIN_Y_DIR, PIN_Y_ENA);

    ICarriage *carriage_x = new Carriage(motor_x, 0, MOTOR_X_MAX_MM, MOTOR_X_PULSE_PER_MM);
    ICarriage *carriage_y = new Carriage(motor_y, 0, MOTOR_Y_MAX_MM, MOTOR_Y_PULSE_PER_MM);

    IIgnitor *ignitor = new Ignitor(PIN_IGNITOR);

    IExperiment *experiment = new Experiment(carriage_x, carriage_y, ignitor);

    // BaseType_t xReturned;
    // TaskHandle_t xHandle = NULL;
    // xReturned = xTaskCreate(
    //                 vSensorReadAndSend,       /* Function that implements the task. */
    //                 "NAME",                   /* Text name for the task. */
    //                 512,                      /* Stack size in words, not bytes. */
    //                 ( void * ) 1,             /* Parameter passed into the task. */
    //                 tskIDLE_PRIORITY,         /* Priority at which the task is created. */
    //                 &xHandle );               /* Used to pass out the created task's handle. */

    auto params = new vConnectorStruct{
        connector,
        config,
        carriage_x,
        carriage_y,
        experiment
    };

    // vConnectorStart(params);

    // return;

    TaskHandle_t xHandle = NULL;
    xTaskCreate(
                    vConnectorStart,       /* Function that implements the task. */
                    "NAME",                   /* Text name for the task. */
                    1 * 51200,                      /* Stack size in words, not bytes. */
                    // 5120,                      /* Stack size in words, not bytes. */
                    (void*)params,             /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,         /* Priority at which the task is created. */
                    &xHandle);               /* Used to pass out the created task's handle. */
}

void loop() {
    // Serial.println("loop");
    // delay(1000);
    // vConnectorStart(nullptr);
    // delay(100);
    // Serial.println("blink up");
    // delay(1000);
    // Serial.println("blink down");
    // delay(1000);
}
