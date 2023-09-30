#pragma once

#include "config.hpp"

class IExperiment {
public:
    virtual bool start(const Config *config) = 0;
    virtual bool is_started() = 0;
    virtual bool stop() = 0;
};
