#pragma once

#include <Arduino.h>
#include <vector>

#include "messages.hpp"
#include "iconnector.hpp"
#include "sensors.hpp"

typedef std::vector<ISensor*> sensors_vector;

struct vSensorTaskParams {
    IConnector *connector;
    // sensors_vector *sensors;
    ISensor *pyrometer;
};

void vSensorReadAndSend(void *vParams) {
    auto params = (vSensorTaskParams*)vParams;

    // for (;;) {
    //     sensors_t *msg = new sensors_t{};
    //     msg->values = new uint16_t[params->sensors->size()];

    //     for (int i = 0; i < params->sensors->size(); i++) {
    //         auto val = params->sensors->at(i)->read();
    //         msg->values[i] = val;
    //     }

    //     SensorsMessage<1> message(msg);
    //     params->connector->sendMessage(message, 0);
    //     delete msg;
    //     delay(500);
    // }

    for (;;) {
        // Serial.println("Loop begin");
        // Serial.flush();
        // printf("Loop begin but in printf");
        auto pyro = params->pyrometer->read();
        // Serial.println("Creating msg");
        sensors_named_t *msg = new sensors_named_t{
            pyro
        };

        // Serial.println("Creating class instance");
        SensorsNamedMessage message(msg);
        // Serial.println("Sending");
        params->connector->sendMessage(message, 0);
        // Serial.println("Deleting");
        // delete msg;
        // Serial.println("Delaying");
        // delay(500);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}