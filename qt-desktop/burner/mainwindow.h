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

enum StepDirection {
    Up = 1,
    Down,
    Left,
    Right
};

enum Axis {
    X = 1,
    Y
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QThread *connector_qthread_ = new QThread;
    QThread *synced_messages_qthread_ = new QThread;
    ConnectorWorker *worker_ = new ConnectorWorker();

    std::atomic<long long> req_id = { 1 };
    long long max_req_id = MAX_REQUEST_ID;

    void setConnectionError(QString error);
    void clearConnectionError();

    void connectCOM(QString port);

    template<typename T>
    T* query(const IMessage *msg, const double timeout_s=3) {
        this->lockControls();
        auto f = this->worker_->sendMessageSyncedFuture(msg, this->generateRequestID(), timeout_s);

        auto h = f.result();
        qDebug() << "Got result in MainWindow::query";
        this->unlockControls();

        if (h == nullptr) {
            return nullptr;
        }

        qDebug() << "create payload origin";
        void *payload_origin = h->get_payload();
        qDebug() << "create payload_copied";
        char *payload_copied = new char[sizeof(T)];
        qDebug() << "memcpy";
        memcpy(payload_copied, payload_origin, sizeof(T));
        qDebug() << "delete h";
        delete h;


        qDebug() << "Create res";
        T *res = (T*)payload_copied;
        qDebug() << "returning";
        return res;
    }

    long long generateRequestID();

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
    void onCurrentPositionReceived(current_position_message_t *pos);
    void onInterruptResponse(response_message_t *resp);
    void onCurrentExperimentFinished(response_message_t *resp);

    void on_pushButton_clicked();

    void on_stepUpButton_clicked();

    void on_stepDownButton_clicked();

    void on_stepLeftButton_clicked();

    void on_stepRightButton_clicked();

    void on_interruptButton_clicked();

    void on_startExperimentButton_clicked();

protected:
     void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    void changeControlsState(bool state);
    void lockControls();
    void unlockControls();

    config_message_t *getConfig();
    current_position_message_t* getCurrentPosition();

    config_message_t *createConfigMessage();
    motor_move_message_t *createMoveMessage(const StepDirection dir, const double stepMm);

    void sendUpdatedConfig(config_message_t *cfg);
    current_position_message_t *sendMove(motor_move_message_t *move);
    response_message_t* sendInterruptMessage();
    response_message_t* sendStartExperimentMessage();

    void stepCnc(StepDirection dir, double stepMm);
    void interruptCnc();
    void startExperimentCnc();

signals:
    void changePage(int index);
};
#endif // MAINWINDOW_H
