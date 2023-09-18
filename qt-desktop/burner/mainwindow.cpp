#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QRegExpValidator>
#include <thread>
#include <QThread>
#include <QSerialPort>
#include <QTime>
#include <QFuture>
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

    QRegExp naturalRegex("^[1-9][0-9]{1,4}$");
    QRegExpValidator *naturalValidator = new QRegExpValidator(naturalRegex, this);

    QRegExp digitRegex("^[-+]?[0-9]{1,5}$");
    QRegExpValidator *digitValidator = new QRegExpValidator(digitRegex, this);

    ui->stepLineEdit->setValidator(naturalValidator);

    ui->currentXInput->setValidator(digitValidator);
    ui->currentYInput->setValidator(digitValidator);

    ui->startXInput->setValidator(digitValidator);
    ui->startYInput->setValidator(digitValidator);
    ui->endYInput->setValidator(digitValidator);

    ui->rapidXSpeedInput->setValidator(naturalValidator);
    ui->rapidYSpeedInput->setValidator(naturalValidator);
    ui->lowXSpeedInput->setValidator(naturalValidator);
    ui->lowYSpeedInput->setValidator(naturalValidator);
    ui->accelXInput->setValidator(naturalValidator);
    ui->accelYInput->setValidator(naturalValidator);

    ui->connectionErrorsLabel->setVisible(false);

    //this->worker_->moveToThread(connector_qthread_);
    connect(this->worker_, &ConnectorWorker::receivedConfig, this, &MainWindow::onConfigReceived);
    connect(this, &MainWindow::changePage, this->ui->stackedOptions, &QStackedWidget::setCurrentIndex);

    //connect(connector_qthread_, SIGNAL(started()), this->worker_, SLOT(start()));
    this->worker_->start();


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

void MainWindow::onCurrentPositionReceived(current_position_message_t* position) {
    ui->currentXInput->setText(QString::number(position->x));
    ui->currentYInput->setText(QString::number(position->y));
}

MainWindow::~MainWindow()
{
    delete ui;
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

config_message_t* MainWindow::getConfig() {
    auto msg = new GetConfigMessage();

    auto cfg = this->query<config_message_t>(msg);
    delete msg;

    if (cfg == nullptr) {
        return nullptr;
    }

    return cfg;
}

current_position_message_t* MainWindow::getCurrentPosition() {
    auto msg = new GetCurrentPositionMessage();

    auto pos = this->query<current_position_message_t>(msg);
    delete msg;

    if (pos == nullptr) {
        return nullptr;
    }

    return pos;
}

void MainWindow::connectCOM(QString port) {
    config_message_t* cfg = this->getConfig();

    if (cfg == nullptr) {
        QString error = "Cannot connect to port " + port;
        this->setConnectionError(error);
        return;
    }

    auto pos = this->getCurrentPosition();

    if (pos == nullptr) {
        QString error = "Cannot get current position";
        this->setConnectionError(error);
        return;
    }

    this->onConfigReceived(cfg);
    this->onCurrentPositionReceived(pos);

    delete[] cfg;
    delete[] pos;

    emit this->changePage(1);
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

    QtConcurrent::run(this, &MainWindow::connectCOM, port);

    return;

    auto msg = new GetConfigMessage();

    qDebug() << "initial packet bytelen" << msg->getByteLen();

#define FUTURE
//#define SYNCED

#ifdef SYNCED
    this->worker_->sendMessageSynced(msg, this->generateRequestID(), 5, [this](IHeader* h) {
        qDebug() << "UI update";
        this->ui->stackedOptions->setCurrentIndex(1);
        qDebug() << "cfg conversion";
        config_message_t *cfg = (config_message_t*)h;
         qDebug() << "Received config msg in ui" << cfg->rapid_speed_x_mm_s;
        this->onConfigReceived(cfg);
         qDebug() << "this updated";
    });
#elif defined(FUTURE)
    auto watcher = new QFutureWatcher<IHeader*>();

    connect(watcher, &QFutureWatcher<IHeader*>::finished, std::bind([&, this](QString p) {
        auto h = watcher->result();
        this->unlockControls();

        if (h == nullptr) {
            qDebug() << "Received nullptr";
//            qDebug() << "concat = " << "Cannot connect to port" + port;
            QString error = "Cannot connect to port" + p;
            qDebug() << "concat = " << error;
            this->setConnectionError(error);
            return;
        }

        this->ui->stackedOptions->setCurrentIndex(1);

        config_message_t *cfg = (config_message_t*)h->get_payload();
         qDebug() << "Received config msg in ui" << cfg->rapid_speed_x_mm_s;
        this->onConfigReceived(cfg);

         delete msg;
    }, port));

    this->lockControls();
    auto f = this->worker_->sendMessageSyncedFuture(msg, this->generateRequestID(), 5);
    watcher->setFuture(f);

#else
    this->worker_->sendMessage(msg, this->generateRequestID());
    this->ui->stackedOptions->setCurrentIndex(1);
    return;
#endif
}

void MainWindow::closeEvent(QCloseEvent *) {
    qDebug() << "Stopping";

    if (this->worker_->isStarted()) {
        this->worker_->stop();
    }
    this->connector_qthread_->quit();
    this->connector_qthread_->wait();
    qDebug() << "Stopped";
}

void MainWindow::changeControlsState(bool state) {
    this->ui->refreshComsButton->setEnabled(state);
    this->ui->selectComButton->setEnabled(state);
    this->ui->comDropdown->setEnabled(state);

    this->ui->startExperimentButton->setEnabled(state);
    this->ui->stepUpButton->setEnabled(state);
    this->ui->stepLeftButton->setEnabled(state);
    this->ui->stepDownButton->setEnabled(state);
    this->ui->stepRightButton->setEnabled(state);

    this->ui->stepLineEdit->setEnabled(state);
    this->ui->startXInput->setEnabled(state);
    this->ui->startYInput->setEnabled(state);
    this->ui->endYInput->setEnabled(state);
    this->ui->rapidXSpeedInput->setEnabled(state);
    this->ui->rapidYSpeedInput->setEnabled(state);
    this->ui->lowXSpeedInput->setEnabled(state);
    this->ui->lowYSpeedInput->setEnabled(state);
    this->ui->accelXInput->setEnabled(state);
    this->ui->accelYInput->setEnabled(state);
}

void MainWindow::lockControls()
{
    this->changeControlsState(false);
}


void MainWindow::unlockControls()
{
    this->changeControlsState(true);
}

void MainWindow::setConnectionError(QString error) {
    this->ui->connectionErrorsLabel->setVisible(true);
    this->ui->connectionErrorsLabel->setText(error);
}

void MainWindow::clearConnectionError() {
    this->ui->connectionErrorsLabel->setVisible(false);
    this->ui->connectionErrorsLabel->setText("No errors");
}

void MainWindow::on_pushButton_clicked()
{
    this->lockControls();
}

config_message_t *MainWindow::createConfigMessage() {
    config_message_t *cfg = new config_message_t;
    cfg->rapid_speed_x_mm_s = this->ui->rapidXSpeedInput->text().toUInt();
    cfg->slow_speed_x_mm_s = this->ui->lowXSpeedInput->text().toUInt();
    cfg->rapid_speed_y_mm_s = this->ui->rapidYSpeedInput->text().toUInt();
    cfg->slow_speed_y_mm_s = this->ui->lowYSpeedInput->text().toUInt();

    cfg->accel_x_mm_s2 = this->ui->accelXInput->text().toUInt();
    cfg->accel_y_mm_s2 = this->ui->accelYInput->text().toUInt();

    cfg->x_mm = this->ui->startXInput->text().toUInt();
    cfg->y1_mm = this->ui->startYInput->text().toUInt();
    cfg->y2_mm = this->ui->endYInput->text().toUInt();

    return cfg;
}

void MainWindow::sendUpdatedConfig(config_message_t *cfg) {
    auto msg = new ConfigMessage(cfg);
    auto resp = this->query<response_message_t>(msg);
    qDebug() << "315";
    if (resp == nullptr) {
        qDebug() << "Cannot connect to COM";
        return;
    }

    delete msg;
    delete[] resp;
}

void MainWindow::stepCnc(StepDirection dir, double stepMm) {
    auto *cfg = this->createConfigMessage();
    this->sendUpdatedConfig(cfg);
}

void MainWindow::on_stepUpButton_clicked()
{
    QtConcurrent::run(this, &MainWindow::stepCnc, StepDirection::Up, this->ui->stepLineEdit->text().toDouble());
}

