#pragma once

#include <stdint.h>
#include <stdexcept>
#include <map>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <set>
#define QT
#ifdef QT
#include <QTime>
#include <QDebug>
#include <QThread>
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
        // #ifdef ARDUINO
        // Serial.println("searching for subscriber");
        // #endif
        if (hdr == nullptr || !this->byte_handler_) return;

        int msg_type_id = hdr->get_msg_type_id();
        
        // #ifdef ARDUINO
        // Serial.println("type=" + msg_type_id);
        // for (auto k : this->subscriptions) {
        //     Serial.println("subcr for " + (int)k.first);
        // }
        // #endif
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
            delay(1);
            // delay(50);
            #endif
            #ifdef QT
//            QThread::usleep(1);
            #endif
            IHeader* hdr = this->tick();

            if (hdr == nullptr) continue;

            long long req_id = hdr->get_request_id();
            

#ifdef QT
            qDebug() << "Got req " << req_id;
#endif

            this->tryCallSubscriber(hdr);
            delete hdr;
            continue;

            bool isExist = this->waiterExists(req_id);
#ifdef QT
            qDebug() << "waiter exists" << isExist;
#endif
            if (!isExist) {
                if (check_subscriptions)
                    this->tryCallSubscriber(hdr);

                continue;
            }

            received_mutex_.lock();
            received_[req_id] = hdr;
            received_mutex_.unlock();
            waiters_mutex_.unlock();
#ifdef QT
            qDebug() << "Added to map";
#endif

            delete hdr;
        }
    }

    void stop() {
        started_ = false;
    }

    bool isStarted() { return started_; }

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
//        return nullptr;

        if (!byte_ref) return nullptr;

        uint8_t byte = *byte_ref;
        #ifdef ARDUINO
        //Serial.println("Received byte " + (int)byte);
        //Serial.flush();
        #endif
#ifdef QT
        //qDebug() << "received byte " << byte;
#endif

        auto result = this->byte_handler_->handleByte(byte);
        // #ifdef ARDUINO
        // if (result)
        //     Serial.println("yes-result=" + String(result->get_msg_type_id()));
        // else
        //     Serial.println("no-result");
        // #endif
//         #ifdef QT
//         if (result)
//             qDebug() << "yes-result=" << result->get_msg_type_id();
//         else
//             qDebug() << "no-result";
//         #endif

        return result;
    }

    bool subscribe(uint8_t msg_id, subscription_type callback) {
        auto subscriber = this->subscriptions.find(msg_id);
        if (subscriber != this->subscriptions.end()) return false;

        this->subscriptions[msg_id] = callback;
        return true;
    }

    void sendMessage(const IMessage &msg, long long req_id=0) {
        auto packet = this->byte_handler_->makePacket(msg, req_id);
        
        auto packet_bytes = packet->to_bytes();
        serial_->writeBytes(packet_bytes, packet->get_total_length());
    }

//     IHeader* sendMessageSynced(const IMessage &msg, long long req_id=0, const double& timeout_s=0) {
//         this->waiters_mutex_.lock();
//         this->waiters_.insert(req_id);
//         this->waiters_mutex_.unlock();
//         sendMessage(msg, req_id);

// #ifdef QT
//         qDebug() << "Started waiting " << req_id;
//         bool is_received = false;

//         QTime dieTime= QTime::currentTime().addSecs(timeout_s);
//         while (!is_received && QTime::currentTime() < dieTime) {
//             this->received_mutex_.lock();
//             is_received = (this->received_.find(req_id) != this->received_.end());
//             this->received_mutex_.unlock();

//             QThread::msleep(1000);
//         }

//         if (QTime::currentTime() > dieTime) {
//             return nullptr;
//         }

//         this->received_mutex_.lock();
// #else

//         std::unique_lock<std::mutex> lock(cv_response_mtx);
//         const auto predicate = [&] {
//             this->received_mutex_.lock();

//             const bool is_received = (this->received_.find(req_id) != this->received_.end());
            
//             if (!is_received) {
//                 this->received_mutex_.unlock();
//             }

//             return is_received;
//         };
//         // depending on the timeout, we may wait a fixed amount of time, or
//         // indefinitely
//         if(timeout_s > 0) {
//             if(!cv_response.wait_for(
//                 lock,
//                 std::chrono::milliseconds(size_t(timeout_s * 1e3)),
//                 predicate)) {
  
//                 return nullptr;
//             }
//         }
//         else {
//             cv_response.wait(lock, predicate);
//         }
// #endif

//         auto res = this->received_.find(req_id)->second;
//         this->received_.erase(req_id);
//         this->received_mutex_.unlock();

//         this->waiters_mutex_.lock();
//         this->waiters_.erase(req_id);
//         this->waiters_mutex_.unlock();

//         return res;

//     }
};
