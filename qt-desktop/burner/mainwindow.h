#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include "DesktopSerial.hpp"
#include "connector.hpp"
#include <thread>
#include <atomic>

#include "settings.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:

    DesktopSerial *serial_ = new DesktopSerial();
    IByteHandler *byte_handler_ = new MyCrsfSerial<REQ_TYPE, LEN_TYPE, MSG_TYPE>();
    Connector *connector_ = new Connector(byte_handler_, serial_);
    std::thread connector_thread_;

    QSerialPort *serial;

    std::atomic<long long> req_id = { 1 };
    long long max_req_id = MAX_REQUEST_ID;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    void startConnector();

private slots:
    void on_pushButton_5_clicked();

    void on_pushButton_8_clicked();

    void on_refreshComsButton_clicked();

    void updateComsDropdown();

    void dropConnection();

    void on_comDropdown_currentIndexChanged(int index);

    void on_selectComButton_clicked();

    long long generateRequestID();

    void readData();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
