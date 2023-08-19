#ifndef DESKTOPSERIAL_HPP
#define DESKTOPSERIAL_HPP

#include <stdint.h>
#include "../../burner/lib/connector/iserial.hpp"
#include <QSerialPort>

class DesktopSerial : public ISerial {
private:
    QSerialPort serial;
public:
    DesktopSerial(unsigned int baud=115200) {
        this->serial.setBaudRate(baud);
    }

    void setPort(QString portName) {
        this->serial.setPortName(portName);
    }

    void open() {
        this->serial.open(QIODevice::ReadWrite);
    }

    void close() {
        this->serial.close();
    }

    uint8_t* readByte() override {
        uint8_t* buf;

        this->serial.bytesAvailable();

//        serial.

//        if (Serial.available() == 0)
//            return nullptr;

//        Serial.readBytes(buf, 1);

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
        this->serial.write(casted, length);
    }
};

#endif // DESKTOPSERIAL_HPP
