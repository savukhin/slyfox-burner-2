#ifndef CONNECTOR_WORKER_HPP
#define CONNECTOR_WORKER_HPP

#include "connector.hpp"
#include "DesktopSerial.hpp"

#include <atomic>

#include <QFuture>
#include <QtCore>
#include <QtConcurrent/QtConcurrent>
#include <QMetaType>

typedef std::function<void(IHeader*)> callbackType;

Q_DECLARE_METATYPE(IMessage*);
Q_DECLARE_METATYPE(IMessage);
Q_DECLARE_METATYPE(std::function<void(IHeader*)>);
class SyncMessageWorker: public QObject{
    Q_OBJECT
private:
    Connector *connector_;

    std::atomic_bool sync_busy_ = { false };
//    IMessage *msg_;
//    long long req_id_=0;
//    double timeout_s_=1;
//    callbackType callback_;

public:
    SyncMessageWorker(Connector *connector): connector_(connector) {
        qRegisterMetaType<IMessage*>("IMessage*");
        qRegisterMetaType<IMessage>("IMessage");
        qRegisterMetaType<std::function<void(IHeader*)>>("std::function<void(IHeader*)>");


//        QObject::connect(this, SIGNAL(sendMessage(IMessage,long long,double,std::function<void(IHeader*)>)),
//                          this, SLOT(sendMessage_(IMessage,long long,double,std::function<void(IHeader*)>)));
        QObject::connect(this, &SyncMessageWorker::sendMessage,
                          this, &SyncMessageWorker::sendMessage_);

        bool res = QObject::connect(this, SIGNAL(Test()),
                          this, SLOT(test_()));
        qDebug() << "connected?" << res;
    }

public:
    Q_SLOT void test_() {
        qDebug() << "TEST WORKS~!!!";
    }

    Q_SLOT void sendMessage_(const IMessage *msg, long long req_id, const double timeout_s, std::function<void(IHeader*)> callback) {
        qDebug() << "Sending message syncworker in thread" << QThread::currentThreadId();
        if (this->sync_busy_) return;
        this->sync_busy_ = true;
         qDebug() << "connector->sendmessagecynced bytelen" << msg->getByteLen();
        auto res = this->connector_->sendMessageSynced(*msg, req_id, timeout_s);
        qDebug() << "connector message sended in thread" << QThread::currentThreadId();

        if (res == nullptr) {
            this->sync_busy_ = false;
//                return false;
            return;
        }

        qDebug() << "Trying callback";
        callback(res);
        qDebug() << "Callback ok";
//        this->connector_->sendMessage(msg, req_id);
        this->sync_busy_ = false;
        return;
    }

public:
    Q_SIGNAL void sendMessage(const IMessage *msg, long long req_id, const double timeout_s, std::function<void(IHeader*)> callback);

    Q_SIGNAL void Test();

//    void stop() {
//        thread->quit();
//        thread->wait();
//    }
};

class AsyncReadWorker: public QObject{
    Q_OBJECT
private:
    Connector *connector_;

public:
    AsyncReadWorker(Connector *connector): connector_(connector) {}

public slots:
    void start() {
        qDebug() << "Started async read worker in thread" << QThread::currentThreadId();
        connector_->start();
        qDebug() << "Stopped async read worker in thread" << QThread::currentThreadId();
    }

    void stop() {
        connector_->stop();
    }
};


struct ConnectorWorker : public QObject
{
    Q_OBJECT
private:

    DesktopSerial *serial_ = new DesktopSerial();
    IByteHandler *byte_handler_ = new MyCrsfSerial<REQ_TYPE, LEN_TYPE, MSG_TYPE>();
    Connector *connector_ = new Connector(byte_handler_, serial_);

//    QThread sync_message_thread_;
    QThread *async_read_thread_ = new QThread;
    QThread *sync_message_thread_ = new QThread;
    SyncMessageWorker *sync_message_worker_ = new SyncMessageWorker(connector_);
    AsyncReadWorker *async_read_worker_ = new AsyncReadWorker(connector_);
    std::atomic_bool sync_busy_ = { false };

private slots:
    void tick() {
        QThread::sleep(1);
    }
public:
    void start() {
        qDebug() << "Started connectorworker in thread" << QThread::currentThreadId();
//        this->connector_->start();

//        while(true) {
//            qDebug() << "tick";
//            QThread::msleep(10);
//        }
//        QThread::currentThread()->quit();
        async_read_thread_->start();
        qDebug() << "Ended connectorworker in thread" << QThread::currentThreadId();
    }
public:
    ConnectorWorker()
    {
//        m_timer.start(0, this);
//        config_message_t *cfg = (config_message_t*)received_msg;
        this->connector_->subscribe(ConfigMessage().get_id(), [&](const void* received_msg, int) {
            config_message_t *cfg = (config_message_t*)received_msg;
            qDebug() << "Received config msg in worker" << cfg->x_mm;
            emit this->receivedConfig(cfg);
        });

        this->connector_->subscribe(CurrentPositionMessage().get_id(), [&](const void* received_msg, int) {
            current_position_message_t *pos = (current_position_message_t*)received_msg;
            emit this->receivedCurrentPosition(pos);
        });

        this->connector_->subscribe(InterruptResponseMessage().get_id(), [&](const void* received_msg, int) {
            response_message_t *pos = (response_message_t*)received_msg;
            emit this->receivedInterruptResponse(pos);
        });

        this->connector_->subscribe(ExperimentFinishedMessage().get_id(), [&](const void* received_msg, int) {
            response_message_t *pos = (response_message_t*)received_msg;
            emit this->receivedExperimentFinished(pos);
        });

        this->connector_->subscribe(SensorsMessage<1>().get_id(), [&](const void* received_msg, int) {
            sensors_t *sensors = (sensors_t*)received_msg;
            emit this->receivedSensors(sensors);
        });

        qDebug() << "constructor connectorworker in thread" << QThread::currentThreadId();
        sync_message_worker_->moveToThread(sync_message_thread_);
        async_read_worker_->moveToThread(async_read_thread_);
        connect(async_read_thread_, SIGNAL(started()), async_read_worker_, SLOT(start()));

        sync_message_thread_->start();
        qDebug() << "constructor's end connectorworker in thread" << QThread::currentThreadId();
    }

    void setPort(QString port) {
        this->serial_->setPort(port);
    }

    bool open() {
        return this->serial_->open();
    }

    void close() {
        return this->serial_->close();
    }

    bool isStarted() {
        return this->connector_->isStarted();
    }

    void stop() {
        this->connector_->stop();
        this->serial_->close();
    }

    void sendMessage(const IMessage& msg, long long req_id) {
        return this->connector_->sendMessage(msg, req_id);
    }

    void sendMessageSynced(const IMessage &msg, long long req_id=0, const double& timeout_s=1,
                               callbackType callback={}) {
//        SyncMessageWorker t(connector_, msg, req_id, timeout_s, callback);
//        connect()
//        t.start();
        qDebug() << "Sending message connectorworker in thread" << QThread::currentThreadId() << " packet len " << msg.getByteLen();
        emit this->sync_message_worker_->sendMessage(&msg, req_id, timeout_s, callback);
        emit this->sync_message_worker_->Test();
        qDebug() << "sended start signal";
//        if ( t.wait((timeout_s + 1) * 1000) ) {
//          // Finished
//            qDebug() << "finished successfully";
//        } else {
//            qDebug() << "dropped";
//          t.terminate();
//          t.wait();
//        }
    }


    IHeader* sendMessageSyncedFuture_(const IMessage *msg, long long req_id=0, const double timeout_s=1) {
        qDebug() << "Sending message concurrent connectorworker in thread" << QThread::currentThreadId();

//        if (this->sync_busy_)
//            return (IHeader*)nullptr;

//        this->sync_busy_ = true;

        auto res = this->connector_->sendMessageSynced(*msg, req_id, timeout_s);
        qDebug() << "connector message sended in thread" << QThread::currentThreadId();

        if (res == nullptr) {
            qDebug() << "Got nullptr in qfuture";
//            this->sync_busy_ = false;
            return (IHeader*)nullptr;
        }


        qDebug() << "got in header with msg id " << res->get_msg_type_id();
//            void *casted = (void*)res;
//            QByteArray byteArray = QByteArray::fromRawData(static_cast<const char*>(casted), 13);
//            qDebug() << "got in concurrent buffer" << byteArray;
      config_message_t *cfg = (config_message_t*)res->get_payload();
      qDebug() << "returning config rapix from worker's future" << cfg->rapid_speed_x_mm_s;


//        this->sync_busy_  = false;

        return res;
    }

    QFuture<IHeader*> sendMessageSyncedFuture(const IMessage *msg, long long req_id=0, const double timeout_s=1) {
        qDebug() << "connectorWorker=>sendMessageSyncedFuture init future in thread" << QThread::currentThreadId();
        qDebug() << "connector->sendMessageSyncedFuture BEFORE CONCURRENT msg bytelen" << msg->getByteLen();
//        QFuture<IHeader*> res = QtConcurrent::run([&,this] {
//            qDebug() << "Sending message concurrent connectorworker in thread" << QThread::currentThreadId();

//            if (this->sync_busy_)
//                return (IHeader*)nullptr;

//            this->sync_busy_ = true;
//             qDebug() << "connector->sendmessagecynced bytelen" << msg->getByteLen();

//            auto res = this->connector_->sendMessageSynced(*msg, req_id, timeout_s);
//            qDebug() << "connector message sended in thread" << QThread::currentThreadId();

//            if (res == nullptr) {
//                qDebug() << "Got nullptr in qfuture";
//                this->sync_busy_ = false;
//                return (IHeader*)nullptr;
//            }


//            qDebug() << "got in header with msg id " << res->get_msg_type_id();
////            void *casted = (void*)res;
////            QByteArray byteArray = QByteArray::fromRawData(static_cast<const char*>(casted), 13);
////            qDebug() << "got in concurrent buffer" << byteArray;
//          config_message_t *cfg = (config_message_t*)res->get_payload();
//          qDebug() << "returning config rapix from worker's future" << cfg->rapid_speed_x_mm_s;


//            this->sync_busy_  = false;

//            return res;
//       });

        QFuture<IHeader*> res = QtConcurrent::run(this, &ConnectorWorker::sendMessageSyncedFuture_, msg, req_id, timeout_s);

        return res;
    }

    virtual ~ConnectorWorker() {};

signals:
    void receivedConfig(config_message_t *cfg);
    void receivedCurrentPosition(current_position_message_t *pos);
    void receivedInterruptResponse(response_message_t *resp);
    void receivedExperimentFinished(response_message_t *resp);
    void receivedSensors(sensors_t *sensors);
};

#endif // CONNECTOR_WORKER_HPP
