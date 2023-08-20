#ifndef DESKTOPSERIAL_HPP
#define DESKTOPSERIAL_HPP

#include <stdint.h>
#include "../../burner/lib/connector/iserial.hpp"
#include <QSerialPort>
#include <QDebug>

class DesktopSerial : public ISerial {
private:

public:
    QSerialPort serial;

    DesktopSerial(unsigned int baud=115200) {
        this->serial.setBaudRate(baud);
    }

    void setPort(QString portName) {
        this->serial.setPortName(portName);
        this->serial.setDataBits(QSerialPort::Data8);
        this->serial.setParity(QSerialPort::NoParity);
        this->serial.setStopBits(QSerialPort::OneStop);
        this->serial.setFlowControl(QSerialPort::NoFlowControl);
    }

    bool open() {
        return this->serial.open(QIODevice::ReadWrite);
    }

    void close() {
        this->serial.close();
    }

    uint8_t* readByte() override {
        QByteArray result = this->serial.read(1);

        if (result.size() == 0)
            return nullptr;

        char val = result[0];

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
        auto res = this->serial.write(casted, length);
        qDebug() << "Wrote " << data;
        for (int i = 0; i < length; i++)
            qDebug() << "[" << i << "]" << (int)data[i];
        qDebug() << "Len " << length;
        qDebug() << "res = " << res;
    }
};

#endif // DESKTOPSERIAL_HPP
