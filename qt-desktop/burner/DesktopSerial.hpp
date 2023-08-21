#ifndef DESKTOPSERIAL_HPP
#define DESKTOPSERIAL_HPP

#include <stdint.h>
#include "../../burner/lib/connector/iserial.hpp"
#include <QSerialPort>
#include <QDebug>
#include <QMutex>


class DesktopSerial : public ISerial, public QObject {
private:
    QMutex mx_;
    QSerialPort serial_;

    QMutex buf_mx_;
    QByteArray buf_;

private slots:
    void readData() {
        const QByteArray data = this->serial_.readAll();
        buf_ += data;
    }

public:
    DesktopSerial(unsigned int baud=115200) {
        this->serial_.setBaudRate(baud);

        connect(&this->serial_, &QSerialPort::readyRead, this, &DesktopSerial::readData);
    }

    void setPort(QString portName) {
        this->serial_.setPortName(portName);
        this->serial_.setDataBits(QSerialPort::Data8);
        this->serial_.setParity(QSerialPort::NoParity);
        this->serial_.setStopBits(QSerialPort::OneStop);
        this->serial_.setFlowControl(QSerialPort::NoFlowControl);
    }

    bool open() {
        return this->serial_.open(QIODevice::ReadWrite);
    }

    void close() override {
        this->serial_.close();
    }

    uint8_t* readByte() override {
        this->buf_mx_.lock();
        if (this->buf_.size() == 0) {
            this->buf_mx_.unlock();
            return nullptr;
        }

        char val = buf_[0];
        buf_.remove(0, 1);
        this->buf_mx_.unlock();

        uint8_t* buf = new uint8_t[1];
        buf[0] = (uint8_t)val;

        return buf;
    }

    void writeByte(uint8_t data) override {
        uint8_t *arr = new uint8_t[1];
        arr[0] = data;
        this->writeBytes(arr, 1);
        delete[] arr;
    }

    void writeBytes(uint8_t *data, long length) override {
        const char* casted = (const char*)data;
        auto res = this->serial_.write(casted, length);
        qDebug() << "Wrote " << data;
        for (int i = 0; i < length; i++)
            qDebug() << "[" << i << "]" << (int)data[i];
        qDebug() << "Len " << length;
        qDebug() << "res = " << res;
    }
};

#endif // DESKTOPSERIAL_HPP
