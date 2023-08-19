#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


#include "DesktopSerial.hpp"
#include "connector.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    DesktopSerial serial;
    Connector connector;

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

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
