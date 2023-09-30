#pragma once

#include <Arduino.h>
#include <vector>

#include "messages.hpp"
#include "iconnector.hpp"
#include "sensors.hpp"

typedef std::vector<ISensor*> sensors_vector;

struct vSensorTaskParams {
    IConnector *connector;
    sensors_vector *sensors;
};

void vSensorReadAndSend(void *vParams) {
    auto params = (vSensorTaskParams*)vParams;

    for (;;) {
        sensors_t *msg = new sensors_t{};
        msg->values = new uint16_t[params->sensors->size()];

        for (int i = 0; i < params->sensors->size(); i++) {
            auto val = params->sensors->at(i)->read();
            msg->values[i] = val;
        }

        SensorsMessage<1> message(msg);
        params->connector->sendMessage(message, 0);
        delete msg;
        delay(500);
    }

    vTaskDelete(NULL);
}