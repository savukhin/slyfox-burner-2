#ifndef CONNECTOR_WORKER_HPP
#define CONNECTOR_WORKER_HPP

#include "connector.hpp"


#include <QtCore>
struct ConnectorWorker : public QObject
{
    Q_OBJECT
private:
    Connector *connector_;

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
    ConnectorWorker(Connector *conn):
        connector_(conn)
    {
//        m_timer.start(0, this);
    }
    virtual ~ConnectorWorker() {};
};

#endif // CONNECTOR_WORKER_HPP
