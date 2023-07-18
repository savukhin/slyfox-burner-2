#pragma once

#include <stdint.h>
#include <stdexcept>
#include <map>
#include <mutex>
#include <atomic>

// #include "messages.hpp"
#include "imessage.hpp"
#include "iconnector.hpp"

#include "my_crsf_serial_interface.hpp"
#include "iserial.hpp"

class Connector : public IConnector {
private:
    // uint8_t* (*readByte) (void);
    IByteHandler* byte_handler_;
    ISerial *serial_;

    std::mutex started_mutex_;
    bool started_ = false;

    std::atomic<long long> new_req_id; 

    // msg_id -> function of pointer to msg and request_id
    std::map<uint8_t, subscription_type> subscriptions;

    void tryCallSubscriber(IHeader *hdr) { 
        if (hdr == nullptr || !this->byte_handler_) return;

        int msg_type_id = hdr->get_msg_type_id();
        auto subscriber = this->subscriptions.find(msg_type_id);
        if (subscriber == this->subscriptions.end()) return;

        int request_id = hdr->get_request_id();
        void* msg = hdr->get_payload();
        subscriber->second(msg, request_id);
    }
    
public:
    Connector(IByteHandler *byte_handler, ISerial *serial) : byte_handler_(byte_handler), serial_(serial) {}

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
        if (this->serial_ == nullptr) 
            throw new std::runtime_error("no readbyte function implemented");
        if (this->byte_handler_ == nullptr) 
            throw new std::runtime_error("no byte handler implemented");
        
        uint8_t *byte_ref = serial_->readByte();
        if (!byte_ref) return nullptr;
        
        uint8_t byte = *byte_ref;

        auto result = this->byte_handler_->handleByte(byte);

        return result;
    }

    bool subscribe(uint8_t msg_id, subscription_type callback) {
        auto subscriber = this->subscriptions.find(msg_id);
        if (subscriber == this->subscriptions.end()) return false;

        this->subscriptions[msg_id] = callback;
        return true;
    }

    void sendMessage(const IMessage &msg, long long req_id=0) {
        auto packet = this->byte_handler_->makePacket(msg, req_id);
        
        auto packet_bytes = packet->to_bytes();
        serial_->writeBytes(packet_bytes, packet->get_length());
    }
};
