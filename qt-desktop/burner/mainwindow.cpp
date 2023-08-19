#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QRegExpValidator>

#include <QSerialPort>
#include <QSerialPortInfo>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QRegExp naturalRegex("^[1-9][0-9]*$");
    QRegExpValidator *naturalValidator = new QRegExpValidator(naturalRegex, this);

    ui->stepLineEdit->setValidator(naturalValidator);

    ui->startXInput->setValidator(naturalValidator);
    ui->endXInput->setValidator(naturalValidator);
    ui->startXInput->setValidator(naturalValidator);

    ui->rapidXSpeedInput->setValidator(naturalValidator);
    ui->rapidYSpeedInput->setValidator(naturalValidator);
    ui->lowXSpeedInput->setValidator(naturalValidator);
    ui->lowYSpeedInput->setValidator(naturalValidator);
    ui->accelXInput->setValidator(naturalValidator);
    ui->accelYInput->setValidator(naturalValidator);

    this->dropConnection();
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


void MainWindow::on_selectComButton_clicked()
{
    QVariant val = ui->comDropdown->currentText();

    ui->stackedOptions->setCurrentIndex(1);
}

