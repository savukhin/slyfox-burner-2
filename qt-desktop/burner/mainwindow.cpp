#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QRegExpValidator>
#include <thread>
#include <QThread>
#include <QSerialPort>
#include <QTime>
#include <QSerialPortInfo>


#include <connector.hpp>
#include "connector_worker.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
#ifdef QT
    qDebug() << "Defined";
#endif
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

    this->connector_qthread_ = new QThread;
//    this->worker = new ConnectorWorker(this->connector_);
    this->worker_->moveToThread(connector_qthread_);
    connect(this->worker_, &ConnectorWorker::receivedConfig, this, &MainWindow::onConfigReceived);


//    connect(&this->serial_->serial, &QSerialPort::readyRead, this, &MainWindow::readData);

    connect(connector_qthread_, SIGNAL(started()), this->worker_, SLOT(start()));
    connector_qthread_->start();
//    connector_qthread_->setPriority(QThread::HighPriority);

    this->dropConnection();
}


void MainWindow::onConfigReceived(config_message_t* cfg) {
//    config_message_t *cfg = (config_message_t*)received_msg;
//    qDebug() << "Received config msg" << cfg->x_mm;

//    qDebug() << "x_mm" << cfg->x_mm;
    ui->startXInput->setText(QString::number(cfg->x_mm));
//    qDebug() << "y1_mm" << cfg->y1_mm;
    ui->startYInput->setText(QString::number(cfg->y1_mm));
//    qDebug() << "y2_mm" << cfg->y2_mm;
    ui->endYInput->setText(QString::number(cfg->y2_mm));

//    qDebug() << "rapid_speed_x_mm_s" << cfg->rapid_speed_x_mm_s;
    ui->rapidXSpeedInput->setText(QString::number(cfg->rapid_speed_x_mm_s));
//    qDebug() << "slow_speed_x_mm_s" << cfg->slow_speed_x_mm_s;
    ui->lowXSpeedInput->setText(QString::number(cfg->slow_speed_x_mm_s));
//    qDebug() << "rapid_speed_y_mm_s" << cfg->rapid_speed_y_mm_s;
    ui->rapidYSpeedInput->setText(QString::number(cfg->rapid_speed_y_mm_s));
//    qDebug() << "slow_speed_y_mm_s" << cfg->slow_speed_y_mm_s;
    ui->lowYSpeedInput->setText(QString::number(cfg->slow_speed_y_mm_s));


//    qDebug() << "accel_x_mm_s2" << cfg->accel_x_mm_s2;
    ui->accelXInput->setText(QString::number(cfg->accel_x_mm_s2));
//    qDebug() << "accel_y_mm_s2" << cfg->accel_y_mm_s2;
    ui->accelYInput->setText(QString::number(cfg->accel_y_mm_s2));

//    qDebug() << "rapid x speed =" << cfg->rapid_speed_x_mm_s << "start x" << cfg->x_mm;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readData() {
//    const QByteArray data = this->serial_->serial.readAll();
//    ui->textBrowser->setText(ui->textBrowser->toPlainText() + "\n" + data);
//    qDebug() << "read all = " << data.toHex(' ');
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
    auto msg = GetConfigMessage();
    this->worker_->sendMessage(msg, this->generateRequestID());
//    ui->stackedOptions->setCurrentIndex((ui->stackedOptions->currentIndex() + 1) % 2);
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
    this->worker_->close();
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

void MainWindow::on_selectComButton_clicked()
{
    QString port = ui->comDropdown->currentText();
    this->worker_->setPort(port);

    qDebug() << "opening";
    if (!this->worker_->open()) {
        qDebug() << "not opened";
    }
    qDebug() << "opened";

//    QThread* thread = new QThread();
//    ConnectorWorker *worker = new ConnectorWorker(this->connector_);
//    worker->moveToThread(thread);

//    connect(thread, SIGNAL(started()), worker, SLOT(start()));
//    thread->start();
//    thread->setPriority(QThread::HighPriority);

    auto msg = GetConfigMessage();

    this->worker_->sendMessage(msg, this->generateRequestID());

    ui->stackedOptions->setCurrentIndex(1);
}

void MainWindow::closeEvent(QCloseEvent *) {
    qDebug() << "Stopping";

    if (this->worker_->isStarted()) {
        this->worker_->stop();
        this->connector_qthread_->quit();
    }
    qDebug() << "Stopped";
}
