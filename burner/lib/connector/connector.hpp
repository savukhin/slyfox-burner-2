#pragma once

#include <stdint.h>
#include <stdexcept>
#include <map>
#include <mutex>

#include "messages.hpp"

#include "my_crsf_serial.hpp"

class Connector {
private:
    uint8_t* (*readByte) (void);
    MyCrsfSerial crsf;

    std::mutex started_mutex_;
    bool started_ = false;

    using subscription_type = std::function<void(void*, request_id_type)>;

    // msg_id -> function of pointer to msg and request_id
    std::map<uint8_t, subscription_type> subscriptions;

    void crsfOnPacket(const crsf_header_t* hdr) {
        if (hdr == nullptr) throw new std::runtime_error("hdr is accidently null");

        auto subscriber = this->subscriptions.find(hdr->msg_type_id);
        if (subscriber == this->subscriptions.end()) return;

        void* msg = (void *)hdr;
        subscriber->second(msg, hdr->request_id);
    }

public:
    Connector() {
        crsf.onPacket = std::bind(&Connector::crsfOnPacket, this, std::placeholders::_1);
    }

    void start() {
        started_ = true;
        while(started_) this->tick();
    }

    void stop() {
        started_ = false;
    }

    bool tick() {
        if (this->readByte == nullptr) 
            throw new std::runtime_error("no readbyte function implemented");
        
        uint8_t *byte_ref = this->readByte();
        if (!byte_ref) return false;
        
        uint8_t byte = *byte_ref;

        crsf.handleByte(byte);

        return true;
    }

    bool subscribe(IMessage msg, subscription_type callback) {
        uint8_t id = msg.get_id();
        auto subscriber = this->subscriptions.find(id);
        if (subscriber == this->subscriptions.end()) return false;

        this->subscriptions[id] = callback;
        return true;
    }
};
