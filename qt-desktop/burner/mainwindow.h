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
#include <QThread>
#include "connector_worker.hpp"
#include "settings.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QThread *connector_qthread_ = new QThread;
    QThread *synced_messages_qthread_ = new QThread;
    ConnectorWorker *worker_ = new ConnectorWorker();

    std::atomic<long long> req_id = { 1 };
    long long max_req_id = MAX_REQUEST_ID;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_5_clicked();

    void on_pushButton_8_clicked();

    void on_refreshComsButton_clicked();

    void updateComsDropdown();

    void dropConnection();

    void on_comDropdown_currentIndexChanged(int index);

    void on_selectComButton_clicked();

    void onConfigReceived(config_message_t *cfg);

    long long generateRequestID();

    void on_pushButton_clicked();

protected:
     void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    void changeControlsState(bool state);
    void lockControls();
    void unlockControls();
};
#endif // MAINWINDOW_H
