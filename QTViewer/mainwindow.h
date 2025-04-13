#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "Mode.h"
#include "motor_thread.h"
#include "motor_sensor_service.h"
#include "MG90S_setting.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ChangeMode_clicked();
    void onModeChanged(SystemMode newMode);
    void on_Exit_clicked();
//     void readFrame();

private:
    Ui::MainWindow *ui;
    MotorThread *motorThread = nullptr;
    MotorSensorService *service = nullptr;
    QTimer *timer;
    // cv::VideoCapture cap;
    // QImage currentFrame;

protected:
    // bool eventFilter(QObject *watched, QEvent *event) override;

};
#endif // MAINWINDOW_H
