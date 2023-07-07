#pragma once

#include "crsf_protocol.hpp"

class IMessage {
public:
    virtual static uint8_t get_id();
    
    virtual void* New(void* source);
};

class BaseMessageReceiver {
public:
    virtual void raiseEvent(void *msg) {}
};

template <class T>
class MessageReceiver : public BaseMessageReceiver {
public:
    virtual void onEvent(T *msg);
    // virtual void onEvent(T *msg);
    
    // void raiseEvent(crsf_header_t *msg) {
    //     T msg = (T)&p->data;
    //     onEvent(msg);
    // }
};
