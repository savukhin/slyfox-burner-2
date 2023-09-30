#pragma once

#include <stdint.h>
#include <stdexcept>
#include <map>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <set>
#ifndef ARDUINO
#define QT
#endif
#ifdef QT
#include <QTime>
#include <QDebug>
#include <QThread>
#include <QMutex>
#endif

// #include "messages.hpp"
#include "../messages/imessage.hpp"
#include "../messages/messages.hpp"
#include "iconnector.hpp"

#include "../crsf/my_crsf_serial_interface.hpp"
#include "iserial.hpp"

class Connector : public IConnector {
private:
    #ifdef ARDUINO
    static unsigned long long get_current_time() {
        return millis();
    }
    #elif defined(QT)
    static unsigned long long get_current_time() {
        return QTime::currentTime().msec();
    }
    #endif

    IByteHandler* byte_handler_;

    std::mutex serial_write_mx_;
    ISerial *serial_;

    unsigned long long max_idle_time_ms_;

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
    Connector(IByteHandler *byte_handler, ISerial *serial, unsigned long long max_idle_time_ms=100) : 
        byte_handler_(byte_handler), serial_(serial), max_idle_time_ms_(max_idle_time_ms) {}

    bool waiterExists(long long req_id) {
        bool res = false;
        this->waiters_mutex_.lock();
        if (waiters_.find(req_id) != waiters_.end()) {
            res = true;
        }
        this->waiters_mutex_.unlock();

        return res;
    }

    void checkIdleAndResetHandler(unsigned long long delta_time_ms) {
        if (delta_time_ms < this->max_idle_time_ms_) return;
        this->byte_handler_->reset();
    }

    void start(bool check_subscriptions=true) {
        started_ = true;
        unsigned long long lastByteTime = get_current_time();
#ifdef QT

      qDebug() << "connector start() called in thread" << QThread::currentThreadId();
#endif
        while(started_) {
            #ifdef ARDUINO
            delay(1);
            // delay(50);
            #endif
            #ifdef QT
//            QThread::usleep(1);
            QThread::yieldCurrentThread();
            #endif
            TickResponse ticked = this->tick();
            auto time = get_current_time();

            if (!ticked.got_byte) {
                this->checkIdleAndResetHandler(time - lastByteTime);
                continue;
            }

            lastByteTime = time;

            IHeader* hdr = ticked.hdr;

            if (hdr == nullptr) continue;

            long long req_id = hdr->get_request_id();   

#ifdef QT
            qDebug() << "Got req " << req_id;
            qDebug() << "Got req type " << hdr->get_msg_type_id();
            qDebug() << "Got req in thread" << QThread::currentThreadId();
#endif

            bool isExist = this->waiterExists(req_id);
#ifdef QT
            qDebug() << "waiter exists" << isExist;
#endif
            if (!isExist) {
                if (check_subscriptions)
                    this->tryCallSubscriber(hdr);

                delete hdr;
                continue;
            }

            received_mutex_.lock();
            received_[req_id] = hdr;
            received_mutex_.unlock();
            waiters_mutex_.unlock();
#ifdef QT
            qDebug() << "Added to map" << received_[req_id]->get_msg_type_id();
#endif
        }
    }

    void stop() {
        started_ = false;
    }

    bool isStarted() { return started_; }

    TickResponse tick() {
        if (this->serial_ == nullptr) 
            throw new std::runtime_error("no readbyte function implemented");
        if (this->byte_handler_ == nullptr) 
            throw new std::runtime_error("no byte handler implemented");

        uint8_t *byte_ref = serial_->readByte();

        if (!byte_ref) return TickResponse{nullptr, false};

        uint8_t byte = *byte_ref;
        #ifdef ARDUINO
        //Serial.println("Received byte " + (int)byte);
        //Serial.flush();
        #endif

        auto result = this->byte_handler_->handleByte(byte);
#ifdef QT
//        qDebug() << "package rapid_speed in connector " << ((config_message_t*)result->get_payload())->rapid_speed_x_mm_s;
#endif
        // #ifdef ARDUINO
        // if (result)
        //     Serial.println("yes-result=" + String(result->get_msg_type_id()));
        // else
        //     Serial.println("no-result");
        // #endif

        return TickResponse{result, true};
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
#ifdef QT
        qDebug() << "write bytes length" << packet->get_total_length() << " while bytelen " << msg.getByteLen();
#endif
// #ifdef ARDUINO
//         printf("locked in send message");
// #endif
        serial_write_mx_.lock();
// #ifdef ARDUINO
//         printf("writeBytes");
// #endif
        serial_->writeBytes(packet_bytes, packet->get_total_length());
        serial_write_mx_.unlock();
    }

     IHeader* sendMessageSynced(const IMessage &msg, long long req_id=0, const double timeout_s=1) {
         this->waiters_mutex_.lock();
         this->waiters_.insert(req_id);
         this->waiters_mutex_.unlock();
#ifdef QT
        qDebug() << "Sending message len" << msg.getByteLen();
#endif
         sendMessage(msg, req_id);
//         return nullptr;

 #ifdef QT
         qDebug() << "Started waiting " << req_id;
         qDebug() << "Sending message in common connector in thread" << QThread::currentThreadId();
         bool is_received = false;

         QTime dieTime= QTime::currentTime().addSecs(timeout_s);
         qDebug() << "dieTime" << dieTime << "current Timr" << QTime::currentTime() << "timeout s =" << timeout_s;
         qDebug() << "is already have" << (this->received_.count(req_id)? " have " : " not have");

         while (started_ && !is_received && QTime::currentTime() < dieTime) {
             this->received_mutex_.lock();
//             is_received = (this->received_.find(req_id) != this->received_.end());
             is_received = (this->received_.count(req_id));
             this->received_mutex_.unlock();

//             QThread::msleep(10);
             QThread::yieldCurrentThread();
//             QThread::usleep(1);
         }

         if (QTime::currentTime() >= dieTime || !started_) {
             qDebug() << "Timeout happened or stated_ =" << started_;
             return nullptr;
         }

         qDebug() << "Found! received = " << is_received << "but count =" << this->received_.count(req_id) << "converted "<< bool(this->received_.count(req_id));

         this->received_mutex_.lock();
 #else

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
 #endif

         auto res = this->received_[req_id];
#ifdef QT
         qDebug() << "got res from received_" << (res ? QString::number(res->get_msg_type_id()) : "nullptr");
#endif
         this->received_.erase(req_id);
         this->received_mutex_.unlock();

         this->waiters_mutex_.lock();
         this->waiters_.erase(req_id);
         this->waiters_mutex_.unlock();

#ifdef QT
      qDebug() << "returning res from connector "<< (res ? QString::number(res->get_msg_type_id()) : "nullptr");
//      if (res != nullptr) {
//        config_message_t *cfg = (config_message_t*)res->get_payload();
//        qDebug() << "returning config rapix from connector" << cfg->rapid_speed_x_mm_s;
//      }
#endif
         return res;

     }
};
