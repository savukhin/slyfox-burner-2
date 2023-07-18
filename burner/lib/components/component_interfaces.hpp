#pragma once

class ISensor {
public:
    virtual uint16_t read() = 0;
};

class IMotor {
public:
    virtual bool pull(bool forward_direction) = 0;
    virtual bool release() = 0;
};

struct MoveResponse {
    float final_position_mm;
    bool interrupted;
};

class ICarriage {
public:
    // Returns final position (if it is higher than borders)
    virtual MoveResponse moveTo(float position, float speed_mm_s, float accell_mm_s2) = 0;
    virtual MoveResponse moveLength(float distance, float speed_mm_s, float accell_mm_s2) = 0;
    virtual float getCurrentPosition() = 0;
    virtual bool isBusy() = 0;
    virtual bool pause() = 0;
    virtual bool resume() = 0;
    virtual bool stop() = 0;
};

class IIgnitor {
public:
    virtual bool ignite() = 0;
    virtual bool stop() = 0;
};
