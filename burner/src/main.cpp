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
#include "sensors_task.hpp"

#define RD_TLS __thread

void setup() {
    Config *config = new Config;

    IByteHandler *byte_handler = new MyCrsfSerial<REQ_TYPE, LEN_TYPE, MSG_TYPE>();

    HardwareSerial *hardware_serial = new HardwareSerial(Serial);

    ISerial *serial = new ArduinoSerial(USB_BAUDRATE, hardware_serial);

    IConnector *connector = new Connector(byte_handler, serial);

    IMotor *motor_x = new ArduinoMotor(PIN_X_PULL, PIN_X_DIR, PIN_X_ENA);
    IMotor *motor_y = new ArduinoMotor(PIN_Y_PULL, PIN_Y_DIR, PIN_Y_ENA);

    ICarriage *carriage_x = new Carriage(motor_x, 0, MOTOR_X_MAX_MM, MOTOR_X_PULSE_PER_MM);
    ICarriage *carriage_y = new Carriage(motor_y, 0, MOTOR_Y_MAX_MM, MOTOR_Y_PULSE_PER_MM);

    IIgnitor *ignitor = new Ignitor(PIN_IGNITOR);

    IExperiment *experiment = new Experiment(carriage_x, carriage_y, ignitor);

    auto pyrometer = new RandomDummySensor();

    // sensors_vector *sensors = new sensors_vector{ pyrometer };
    // ISensor *sensors[] = { new RandomDummySensor() };
    // ISensor *sensors[] = { new PhotoResistor(PIN_PHOTO_RESISTOR) };

    auto sensorTaskParams = new vSensorTaskParams {
        connector,
        pyrometer
    };

    TaskHandle_t xSensorsHandle = NULL;
    xTaskCreate(
                    vSensorReadAndSend,       /* Function that implements the task. */
                    "NAME2",                   /* Text name for the task. */
                    51200,                      /* Stack size in words, not bytes. */
                    sensorTaskParams,             /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,         /* Priority at which the task is created. */
                    &xSensorsHandle );               /* Used to pass out the created task's handle. */

    auto connectorParams = new vConnectorStruct{
        connector,
        config,
        carriage_x,
        carriage_y,
        experiment
    };

    TaskHandle_t xConnectorHandle = NULL;
    xTaskCreate(
                    vConnectorStart,       /* Function that implements the task. */
                    "NAME",                   /* Text name for the task. */
                    1 * 51200,                      /* Stack size in words, not bytes. */
                    // 5120,                      /* Stack size in words, not bytes. */
                    (void*)connectorParams,             /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,         /* Priority at which the task is created. */
                    &xConnectorHandle);               /* Used to pass out the created task's handle. */
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
