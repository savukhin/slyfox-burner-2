#pragma once

#include <stdint.h>
#include <stdexcept>
#include <map>
#include <mutex>

// #include "messages.hpp"
#include "imessage.hpp"

#include "my_crsf_serial_interface.hpp"
#include "iserial.hpp"

class IConnector {
protected:
    using subscription_type = std::function<void(void*, long)>;

public:
    virtual void start(bool check_subscriptions=true) = 0;
    virtual void stop() = 0;
    virtual IHeader* tick() = 0;

    virtual bool subscribe(uint8_t msg_id, subscription_type callback) = 0;

    virtual void sendMessage(const IMessage &msg, long long req_id=0) = 0;
};
