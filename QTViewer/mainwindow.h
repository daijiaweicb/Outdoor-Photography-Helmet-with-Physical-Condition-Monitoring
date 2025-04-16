#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <mutex>
#include "Mode.h"
#include "motor_thread.h"
#include "motor_sensor_service.h"
#include "libcam2opencv.h"
#include "fatigue_detector.h"

// To avoid macro conflicts between Qt and libcamera on keywords like signals, slots, and emit, you should set:
// set(QT_NO_KEYWORDS ON) in CMakeLists.txt.
// Then, replace the following keywords with their safe alternatives:
// signals: should be replaced with Q_SIGNALS:
// slots: should be replaced with Q_SLOTS:
// emit should be replaced with Q_EMIT
// This allows you to safely include libcamera-related headers in .h files, while preserving Qt’s signal-slot mechanism and avoiding compilation errors. 
// This approach is recommended when integrating libcamera with a Qt application.
// Reference: https://forums.raspberrypi.com/viewtopic.php?t=331741#p1985489


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow,public Libcam2OpenCV::Callback
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void hasFrame(const cv::Mat &frame, const libcamera::ControlList &metadata) override;

private Q_SLOTS:
    void on_ChangeMode_clicked();
    void onModeChanged(SystemMode newMode);
    void on_Exit_clicked();
    void on_btn_record_clicked();

private:
    Ui::MainWindow *ui;
    MotorThread *motorThread = nullptr;
    MotorSensorService *service = nullptr;
    QTimer *timer;
    Libcam2OpenCV *cam = nullptr; 
    QImage currentFrame;
    cv::VideoWriter videoWriter;
    bool isRecording = false;
    FatigueDetector detector;
    std::mutex writerMutex; 
};
#endif // MAINWINDOW_H
