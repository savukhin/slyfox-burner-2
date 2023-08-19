#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QRegExpValidator>
#include <thread>

#include <QSerialPort>
#include <QTime>
#include <QSerialPortInfo>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "Started";

    QRegExp naturalRegex("^[1-9][0-9]*$");
    QRegExpValidator *naturalValidator = new QRegExpValidator(naturalRegex, this);

    ui->stepLineEdit->setValidator(naturalValidator);

    ui->startXInput->setValidator(naturalValidator);
    ui->startYInput->setValidator(naturalValidator);
    ui->endYInput->setValidator(naturalValidator);

    ui->rapidXSpeedInput->setValidator(naturalValidator);
    ui->rapidYSpeedInput->setValidator(naturalValidator);
    ui->lowXSpeedInput->setValidator(naturalValidator);
    ui->lowYSpeedInput->setValidator(naturalValidator);
    ui->accelXInput->setValidator(naturalValidator);
    ui->accelYInput->setValidator(naturalValidator);

    connect(&this->serial_->serial, &QSerialPort::readyRead, this, &MainWindow::readData);

    this->dropConnection();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readData() {
    const QByteArray data = this->serial_->serial.readAll();
    ui->textBrowser->setText(ui->textBrowser->toPlainText() + "\n" + data);
}


void MainWindow::on_pushButton_5_clicked()
{
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Test", "Quit?",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
        qDebug() << qVersion();
        QApplication::quit();
      } else {
        qDebug() << "Yes was *not* clicked";
      }
}


void MainWindow::on_pushButton_8_clicked()
{
    ui->stackedOptions->setCurrentIndex((ui->stackedOptions->currentIndex() + 1) % 2);
}

void MainWindow::updateComsDropdown() {
    ui->comDropdown->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        list << info.portName();
      ui->comDropdown->addItem(list.first());
    }

    ui->selectComButton->setVisible(false);
}

void MainWindow::dropConnection() {
    ui->stackedOptions->setCurrentIndex(0);
    this->serial_->close();
    this->updateComsDropdown();
}

void MainWindow::on_refreshComsButton_clicked()
{
    this->updateComsDropdown();
}


void MainWindow::on_comDropdown_currentIndexChanged(int)
{
    ui->selectComButton->setVisible(true);
}

long long MainWindow::generateRequestID() {
    this->req_id = ((this->req_id + 1) % this->max_req_id) + 1;
    return this->req_id;
}

void MainWindow::startConnector() {
//    this->connector_thread_ = std::thread{&Connector::start, this->connector_};
    this->connector_->start();
}


void MainWindow::on_selectComButton_clicked()
{
    QString port = ui->comDropdown->currentText();
////    const SettingsDialog::Settings p = m_settings->settings();
//    serial->setPortName(port);
//    serial->setBaudRate(115200);
////    serial->setDataBits(QSerialPort::Data8);
////    serial->setParity(QSerialPort::NoParity);
////    serial->setStopBits(QSerialPort::OneStop);
////    serial->setFlowControl(QSerialPort::NoFlowControl);
//    if (serial->open(QIODevice::ReadWrite)) {
//        ui->textBrowser->setEnabled(true);
////        ui->textBrowser->setLocalEchoEnabled(p.localEchoEnabled);
//    } else {
//        QMessageBox::critical(this, tr("Error"), serial->errorString());
//}

//    return;
    this->serial_->setPort(port);
//    serial->setBaudRate(115200);

    qDebug() << "opening";
    if (!this->serial_->open()) {
        qDebug() << "not opened";
    }
    qDebug() << "opened";
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    qDebug() << "new thread";
//    this->connector_thread_ = std::thread(&MainWindow::startConnector, this);
    qDebug() << "inited";

    auto msg = GetConfigMessage();
    auto id = msg.get_id();

    this->connector_->sendMessage(msg);
    return;


    auto cfg_hdr = this->connector_->sendMessageSynced(GetConfigMessage(), this->generateRequestID(), 2);

    qDebug() << "print";
    if (cfg_hdr->get_msg_type_id() != ConfigMessage().get_id()) {
        // TODO: proccess error
    }

    config_message_t *cfg = (config_message_t*)cfg_hdr->get_payload();

    ui->startXInput->setText(QString::number(cfg->x_mm));
    ui->startYInput->setText(QString::number(cfg->y1_mm));
    ui->endYInput->setText(QString::number(cfg->y2_mm));

    ui->rapidXSpeedInput->setText(QString::number(cfg->rapid_speed_x_mm_s));
    ui->lowXSpeedInput->setText(QString::number(cfg->slow_speed_x_mm_s));
    ui->rapidYSpeedInput->setText(QString::number(cfg->rapid_speed_y_mm_s));
    ui->lowYSpeedInput->setText(QString::number(cfg->slow_speed_y_mm_s));

    ui->accelXInput->setText(QString::number(cfg->accel_x_mm_s2));
    ui->accelYInput->setText(QString::number(cfg->accel_y_mm_s2));

    ui->stackedOptions->setCurrentIndex(1);
}

