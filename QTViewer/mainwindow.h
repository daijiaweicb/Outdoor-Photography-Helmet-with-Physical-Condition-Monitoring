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
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief GUI controller for real-time fatigue detection system.
 *
 * The MainWindow class integrates:
 * - Real-time video capture using libcamera + OpenCV
 * - Servo & sensor management via MotorSensorService
 * - Stepper motor mode control (Normal/FatigueDetection)
 * - Fatigue detection using EAR-based analysis
 * - Video recording with timestamped filenames
 *
 * It also manages signal-slot communication to update UI in real-time.
 */
class MainWindow : public QMainWindow, public Libcam2OpenCV::Callback
{
    Q_OBJECT

public:
    /**
     * @brief Constructor. Initializes UI and starts all subsystems.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor. Ensures resources and devices are safely stopped.
     */
    ~MainWindow();

    /**
     * @brief Receives frames from camera callback.
     * Performs either video preview or fatigue detection depending on system mode.
     */
    void hasFrame(const cv::Mat &frame, const libcamera::ControlList &metadata) override;
    /**
     * @brief Shutdown the application
     */

    void safeShutdown();

private Q_SLOTS:
    /**
     * @brief Called when mode change button is clicked.
     */
    void on_ChangeMode_clicked();

    /**
     * @brief Updates UI based on system mode changes.
     */
    void onModeChanged(SystemMode newMode);

    /**
     * @brief Handles clean program shutdown when exit button is clicked.
     */
    void on_Exit_clicked();

    /**
     * @brief Starts or stops video recording based on user input.
     */
    void on_btn_record_clicked();

private:
    Ui::MainWindow *ui;                    // Qt-generated UI class
    MotorThread *motorThread = nullptr;    // Thread for stepper motor transitions
    MotorSensorService *service = nullptr; // Wrapper for servo and sensor system
    QTimer *timer;                         // Clock timer for UI
    Libcam2OpenCV *cam = nullptr;          // Camera interface
    QImage currentFrame;                   // Latest captured image frame
    cv::VideoWriter videoWriter;           // OpenCV video recorder
    bool isRecording = false;              // Indicates recording status
    FatigueDetector detector;              // Fatigue detection processor
    std::mutex writerMutex;                // Ensures thread-safe video writing
};
#endif // MAINWINDOW_H
