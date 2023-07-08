#pragma once

#include <stdint.h>
#include <stdexcept>
#include <map>
#include <mutex>

// #include "messages.hpp"
#include "imessage.hpp"

#include "my_crsf_serial_interface.hpp"

class Connector {
private:
    uint8_t* (*readByte) (void);
    IByteHandler byte_handler_;

    std::mutex started_mutex_;
    bool started_ = false;

    using subscription_type = std::function<void(void*, int)>;

    // msg_id -> function of pointer to msg and request_id
    std::map<uint8_t, subscription_type> subscriptions;

    void tryCallSubscriber(IHeader *hdr) { 
        if (hdr == nullptr) return;

        int msg_type_id = hdr->get_msg_id();
        auto subscriber = this->subscriptions.find(msg_type_id);
        if (subscriber == this->subscriptions.end()) return;

        int request_id = hdr->get_request_id();
        void* msg = hdr->get_data();
        subscriber->second(msg, request_id);
    }
    
public:
    Connector(IByteHandler byte_handler) : byte_handler_(byte_handler) {}

    void start(bool check_subscriptions=true) {
        started_ = true;
        while(started_) {
            auto hdr = this->tick();

            this->tryCallSubscriber(hdr);
        }
    }

    void stop() {
        started_ = false;
    }

    IHeader* tick() {
        if (this->readByte == nullptr) 
            throw new std::runtime_error("no readbyte function implemented");
        
        uint8_t *byte_ref = this->readByte();
        if (!byte_ref) return nullptr;
        
        uint8_t byte = *byte_ref;

        auto result = this->byte_handler_.handleByte(byte);

        return result;
    }

    bool subscribe(IMessage msg, subscription_type callback) {
        uint8_t id = msg.get_id();
        auto subscriber = this->subscriptions.find(id);
        if (subscriber == this->subscriptions.end()) return false;

        this->subscriptions[id] = callback;
        return true;
    }
};
