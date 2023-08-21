#ifndef CONNECTOR_WORKER_HPP
#define CONNECTOR_WORKER_HPP

#include "connector.hpp"
#include "DesktopSerial.hpp"


#include <QtCore>
struct ConnectorWorker : public QObject
{
    Q_OBJECT
private:

    DesktopSerial *serial_ = new DesktopSerial();
    IByteHandler *byte_handler_ = new MyCrsfSerial<REQ_TYPE, LEN_TYPE, MSG_TYPE>();
    Connector *connector_ = new Connector(byte_handler_, serial_);
private slots:
    void tick() {
        QThread::sleep(1);
    }

    void start() {
        qDebug() << "Started thread";
        this->connector_->start();
//        while(true) {
//            qDebug() << "tick";
//            QThread::msleep(10);
//        }
        QThread::currentThread()->quit();
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

    virtual ~ConnectorWorker() {};

signals:
    void receivedConfig(config_message_t *cfg);
};

#endif // CONNECTOR_WORKER_HPP
