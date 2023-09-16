#define QT

#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QLocale>
#include <iostream>
#include <QTranslator>
#include <QtCore/qglobal.h>
#include <QDebug>
#include <QMetaType>
#include "connector.hpp"

class MyClass {
public:
    virtual uint8_t get_id() const = 0;
};

//Q_DECLARE_METATYPE(IMessage);
//Q_DECLARE_METATYPE(std::function<void(IHeader*)>);
//Q_DECLARE_METATYPE(callbackType);
//qRegisterMetaType<Connector>();
//qRegisterMetaType<QVector<unsigned char> >("QVector<unsigned char>");
//qRegisterMetaType<A>("A");
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

//    qRegisterMetaType<Connector>();
//    qRegisterMetaType<QVector<unsigned char> >("QVector<unsigned char>");
//    qRegisterMetaType<MyClass>("MyClass");
//    qRegisterMetaType<IMessage>("IMessage");
//    qRegisterMetaType<std::function<void(IHeader*)>>("std::function<void(IHeader*)>");
//    qRegisterMetaType<callbackType>("callbackType");


    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "burner_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
