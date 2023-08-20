QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ../../burner/lib/connector/arduino_serial.hpp \
    ../../burner/lib/connector/connector.hpp \
    ../../burner/lib/connector/iconnector.hpp \
    ../../burner/lib/connector/iserial.hpp \
    ../../burner/lib/crsf/crc8.hpp \
    ../../burner/lib/crsf/crsf_header.hpp \
    ../../burner/lib/crsf/crsf_protocol.hpp \
    ../../burner/lib/crsf/my_crsf_serial.hpp \
    ../../burner/lib/crsf/my_crsf_serial_interface.hpp \
    ../../burner/lib/messages/imessage.hpp \
    ../../burner/lib/messages/messages.hpp \
    DesktopSerial.hpp \
    connector.hpp \
    connector_worker.hpp \
    mainwindow.h \
    settings.hpp

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    burner_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations
CONFIG += console
#CONFIG += serialport

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
