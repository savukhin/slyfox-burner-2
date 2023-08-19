#pragma once

#include <stdint.h>
#include <stdexcept>
#include <map>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <set>

#ifdef QT
#include <QDebug>
#endif

// #include "messages.hpp"
#include "../messages/imessage.hpp"
#include "iconnector.hpp"

#include "../crsf/my_crsf_serial_interface.hpp"
#include "iserial.hpp"

class Connector : public IConnector {
private:
    // uint8_t* (*readByte) (void);
    IByteHandler* byte_handler_;
    ISerial *serial_;

    // std::mutex started_mutex_;
    std::atomic_bool started_ = { false };

    std::mutex received_mutex_;
    std::map<long long, IHeader*> received_;
    std::mutex waiters_mutex_;
    std::set<long long> waiters_;
    std::condition_variable cv_response;
    std::mutex cv_response_mtx;

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

    bool waiterExists(long long req_id) {
        bool res = false;
        this->waiters_mutex_.lock();
        if (waiters_.find(req_id) != waiters_.end()) {
            res = true;
        }
        this->waiters_mutex_.unlock();

        return res;
    }

    void start(bool check_subscriptions=true) {
        started_ = true;
        while(started_) {
            #ifdef ARDUINO
            delay(250);
            #endif
            IHeader* hdr = this->tick();

            if (hdr == nullptr) continue;

            long long req_id = hdr->get_request_id();

            bool isExist = this->waiterExists(req_id);

            if (!isExist) {
                if (check_subscriptions)
                    this->tryCallSubscriber(hdr);

                continue;
            }

            received_mutex_.lock();
            received_[req_id] = hdr;
            received_mutex_.unlock();

            

            delete hdr;
        }
    }

    void stop() {
        started_ = false;
    }

    IHeader* tick() {
        #ifdef ARDUINO
        // Serial.println("tick");
        // Serial.flush();
        #endif

        if (this->serial_ == nullptr) 
            throw new std::runtime_error("no readbyte function implemented");
        if (this->byte_handler_ == nullptr) 
            throw new std::runtime_error("no byte handler implemented");

        uint8_t *byte_ref = serial_->readByte();

        if (!byte_ref) return nullptr;

        uint8_t byte = *byte_ref;
        #ifdef ARDUINO
        //Serial.println("Received byte " + (int)byte);
        //Serial.flush();
        #endif
#ifdef QT
        qDebug() << "received byte " << byte;
#endif

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
        auto id = msg.get_id();
        auto packet = this->byte_handler_->makePacket(msg, req_id);
        
        auto packet_bytes = packet->to_bytes();
        serial_->writeBytes(packet_bytes, packet->get_total_length());
    }

    IHeader* sendMessageSynced(const IMessage &msg, long long req_id=0, const double& timeout_s=0) {
        sendMessage(msg, req_id);

        
        std::unique_lock<std::mutex> lock(cv_response_mtx);
        const auto predicate = [&] {
            this->received_mutex_.lock();

            const bool is_received = (this->received_.find(req_id) != this->received_.end());
            
            if (!is_received) {
                this->received_mutex_.unlock();
            }

            return is_received;
        };
        // depending on the timeout, we may wait a fixed amount of time, or
        // indefinitely
        if(timeout_s > 0) {
            if(!cv_response.wait_for(
                lock,
                std::chrono::milliseconds(size_t(timeout_s * 1e3)),
                predicate)) {
  
                return nullptr;
            }
        }
        else {
            cv_response.wait(lock, predicate);
        }

        auto res = this->received_.find(req_id)->second;
        this->received_mutex_.unlock();
        return res;

    }
};
