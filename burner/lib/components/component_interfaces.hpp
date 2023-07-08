#pragma once

class ISensor {
public:
    virtual float read() = 0;
};

class IMotor {
public:
    virtual bool pull(bool forward_direction) = 0;
    virtual bool release() = 0;
};

class ICarriage {
public:
    // Returns final position (if it is higher than borders)
    virtual float moveTo(float position, float speed_mm_s, float accell_mm_s2) = 0;
    virtual float moveLength(float distance, float speed_mm_s, float accell_mm_s2) = 0;
    virtual float getCurrentPosition() = 0;
    virtual bool isBusy() = 0;
};
