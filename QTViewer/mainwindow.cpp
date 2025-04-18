#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "libcam2opencv.h"
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QIcon>
#include <atomic>
#include <thread>

/**
 * @brief Constructs the main window and initializes all subsystems.
 *
 * - Sets up UI labels and clock timer
 * - Starts MotorSensorService (servo & MPU6050)
 * - Registers Qt slots for temperature/angle
 * - Starts libcamera video stream (640x480, 30 FPS)
 * - Loads background image into UI
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), detector("../../models/")
{
    ui->setupUi(this);
    ui->label_status->setText("Current Mode：Normal");
    ui->label_video->setScaledContents(true);
    connect(ui->ChangeMode, &QPushButton::clicked, this, &MainWindow::on_ChangeMode_clicked);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]()
            {
        QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        ui->label_time->setText("Time:" + now); });

    service = new MotorSensorService();
    service->start();
    connect(service->getMotorControl(), &MotorControlQT::temperatureUpdated,
            this, [=](float temp)
            { ui->label_temp->setText(QString("Temp: %1 ℃").arg(temp, 0, 'f', 1)); });

    connect(service->getMotorControl(), &MotorControlQT::angleUpdate,
            this, [=](float angle)
            { ui->label_angle->setText(QString("Angle: %1°").arg(angle, 0, 'f', 1)); });

    timer->start(1000);

    cam = new Libcam2OpenCV();
    cam->registerCallback(this);
    Libcam2OpenCVSettings settings;
    settings.width = 640;
    settings.height = 480;
    settings.framerate = 30;
    cam->start(settings);

    QString imagePath = "../../QTViewer/images/background.png";
    QPixmap pix(imagePath);

    if (!pix.isNull())
    {
        QStandardItemModel *model = new QStandardItemModel(this);
        QStandardItem *item = new QStandardItem();
        item->setIcon(QIcon(pix));
        item->setTextAlignment(Qt::AlignCenter);
        model->appendRow(item);

        ui->label->setPixmap(QPixmap(imagePath));
        ui->label->setScaledContents(true);
    }
    else
    {
        qDebug() << "Background load failed" << imagePath;
    }
}

/**
 * @brief Gracefully shuts down camera and motor.
 */
MainWindow::~MainWindow()
{
    if (cam)
    {
        cam->stop();
        delete cam;
    }
    if (g_systemMode == SystemMode::FatigueDetection)
    {
        MotorThread exitMotor(this);
        g_systemMode = SystemMode::Temp;
        exitMotor.run();
    }
    delete ui;
}

/**
 * @brief Starts motor thread to handle mode change (Normal ↔ FatigueDetection).
 * Ensures motor is not already running before transition.
 */
void MainWindow::on_ChangeMode_clicked()
{
    if (motorThread && motorThread->isRunning())
    {
        qDebug() << "motor is running....Do not operate....";
        return;
    }

    motorThread = new MotorThread(this);

    connect(motorThread, &MotorThread::modeChanged,
            this, &MainWindow::onModeChanged);

    connect(motorThread, &MotorThread::finished, this, [this]()
            {
        motorThread->deleteLater();
        motorThread = nullptr; });

    motorThread->start();
}

/**
 * @brief Updates status label on mode change.
 */
void MainWindow::onModeChanged(SystemMode newMode)
{
    if (newMode == SystemMode::Normal)
    {
        ui->label_status->setText("Current Mode：Normal");
        ui->label_fati->setText("                      ");
    }
    else if (newMode == SystemMode::FatigueDetection)
        ui->label_status->setText("Current Mode：FatigueDetection");
    else if (newMode == SystemMode::Temp)
        ui->label_status->setText("Mode is changing .......");
}

void MainWindow::on_Exit_clicked()
{
    auto reply = QMessageBox::question(this, "Exit Confirmation", "Are you sure you want to exit?",
                                       QMessageBox::No | QMessageBox::Yes);
    if (reply == QMessageBox::Yes)
    {
        QApplication::quit();
    }
}

/**
 * @brief Frame handler called on each new video frame.
 * Behavior depends on system mode:
 * - Normal: just display the image (flipped)
 * - FatigueDetection: run EAR-based fatigue detection in a separate thread
 * - Temp: notify UI that mode is switching
 */
void MainWindow::hasFrame(const cv::Mat &frame, const libcamera::ControlList &)
{

    static std::atomic<bool> busy = false;
    if (g_systemMode == SystemMode::Normal)
    {
        cv::Mat flipped;
        cv::flip(frame, flipped, -1);
        QImage qimg(flipped.data, flipped.cols, flipped.rows, flipped.step, QImage::Format_RGB888);
        QImage safeFrame = qimg.copy();
        currentFrame = safeFrame;

        if (isRecording && videoWriter.isOpened())
        {
            std::lock_guard<std::mutex> lock(writerMutex);
            cv::Mat bgr;
            cv::cvtColor(frame, bgr, cv::COLOR_RGB2BGR);
            videoWriter.write(bgr);
        }

        QMetaObject::invokeMethod(this, [this, safeFrame]()
                                  { ui->label_video->setPixmap(QPixmap::fromImage(safeFrame)); });
    }
    else if (g_systemMode == SystemMode::FatigueDetection)
    {
        static int total = 0;
        static int fati_time = 0;
        static int fati_per = 0;
        cv::Mat detectInput = frame;
        if (busy)
            return;
        busy = true;

        std::thread([this, detectInput]()
                    {
        cv::Mat output;
        bool drowsy = detector.detect(detectInput, output);

        if (output.empty()) {
            QMetaObject::invokeMethod(this, [this]() {
                busy = false;
            });
            return;
        }     
        QImage qimg(output.data, output.cols, output.rows, output.step, QImage::Format_RGB888);
        QImage safeFrame = qimg.copy();
        currentFrame = safeFrame;
        total++;
        
        if (isRecording && videoWriter.isOpened())
        {
            std::lock_guard<std::mutex> lock(writerMutex);
            cv::Mat bgr;
            cv::cvtColor(detectInput, bgr, cv::COLOR_RGB2BGR);
            videoWriter.write(bgr);
        }
        QMetaObject::invokeMethod(this, [this, safeFrame, drowsy]() {
            ui->label_video->setPixmap(QPixmap::fromImage(safeFrame));
            ui->label_fati->setText(drowsy ? "Fatigue Detected" : "Normal");
            if(drowsy == 1)
            {
                fati_time++;
            }
            float fati_per = static_cast<float>(fati_time) / total * 100.0f;
            int fati_per_int = static_cast<int>(std::round(fati_per)); 
            ui->label_per->setText(QString("Fatigue Ratio: %1%").arg(fati_per_int));
            busy = false;
        }); })
            .detach();
    }
    else if (g_systemMode == SystemMode::Temp)
    {
        QMetaObject::invokeMethod(this, [this]()
                                  { ui->label_video->setText("Mode is changing, please wait ....."); });
    }
}

/**
 * @brief Starts or stops video recording to disk.
 *
 * - Uses timestamped filename in `~/Videos/`
 * - Recording FPS depends on mode (FatigueDetection = 10, Normal = 20)
 * - Uses mutex to ensure safe writing
 */
void MainWindow::on_btn_record_clicked()
{
    if (!isRecording)
    {
        if (currentFrame.isNull() || currentFrame.width() <= 0 || currentFrame.height() <= 0)
        {
            QMessageBox::warning(this, "Recording Error", "No video frame available. Please wait for camera feed.");
            return;
        }

        QString filename = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".avi";
        QString saveDir = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
        QDir().mkpath(saveDir);
        QString fullPath = saveDir + "/" + filename;
        qDebug() << "saveDir =" << saveDir;
        qDebug() << "fullPath =" << fullPath;

        int fps = (g_systemMode == SystemMode::FatigueDetection) ? 10 : 20;
        QSize size = currentFrame.size();

        videoWriter.open(fullPath.toStdString(),
                         cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                         fps,
                         cv::Size(size.width(), size.height()));

        if (!videoWriter.isOpened())
        {
            qDebug() << "Failed to open video file for writing.";
            return;
        }

        isRecording = true;
        ui->btn_record->setText("Stop Recording");
    }
    else
    {
        isRecording = false;
        std::lock_guard<std::mutex> lock(writerMutex);
        if (videoWriter.isOpened())
        {
            videoWriter.release();
        }
        ui->btn_record->setText("Start Recording");

        QMessageBox::information(this, "Recording Saved", "Video has been saved.");
    }
}
