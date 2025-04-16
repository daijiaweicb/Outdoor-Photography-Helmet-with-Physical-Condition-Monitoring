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
#include <atomic>
#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
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
    startDetectionThread();
}

MainWindow::~MainWindow()
{
    if (cam)
    {
        cam->stop();
        delete cam;
    }
    running = false;
    if (detectionThread.joinable())
    {
        detectionThread.join();
    }
    delete ui;
}

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

void MainWindow::onModeChanged(SystemMode newMode)
{
    if (newMode == SystemMode::Normal)
        ui->label_status->setText("Current Mode：Normal");
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

void MainWindow::hasFrame(const cv::Mat &frame, const libcamera::ControlList &)
{
    std::lock_guard<std::mutex> lock(latestFrameMutex);
    latestFrame = frame.clone();
}

void MainWindow::startDetectionThread()
{
    detectionThread = std::thread([this]() {
        int frameCounter = 0;
        while (running) {
            if (analyzing.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            cv::Mat frameCopy;
            {
                std::lock_guard<std::mutex> lock(latestFrameMutex);
                if (latestFrame.empty()) continue;
                frameCopy = latestFrame.clone();
            }

            SystemMode mode = g_systemMode;

            if (mode == SystemMode::Normal) {
                QImage qimg(frameCopy.data, frameCopy.cols, frameCopy.rows, frameCopy.step, QImage::Format_RGB888);
                QImage safeFrame = qimg.copy();

                if (isRecording && videoWriter.isOpened()) {
                    videoWriter.write(frameCopy);
                }

                QMetaObject::invokeMethod(this, [this, safeFrame]() {
                    ui->label_video->setPixmap(QPixmap::fromImage(safeFrame));
                });

                std::this_thread::sleep_for(std::chrono::milliseconds(30)); 
            }
            else if (mode == SystemMode::FatigueDetection) {
                frameCounter++;
                
                if (frameCounter % 3 != 0) {
                    QImage preview(frameCopy.data, frameCopy.cols, frameCopy.rows, frameCopy.step, QImage::Format_RGB888);
                    QImage preFrame = preview.copy();
                    QMetaObject::invokeMethod(this, [this, preFrame]() {
                        ui->label_video->setPixmap(QPixmap::fromImage(preFrame));
                    });
                    continue;
                }
            
                if (analyzing) continue; 
                analyzing = true;
            
                cv::Mat detectInput = frameCopy.clone();
            
                threadPool.enqueue([this, detectInput]() {
                    cv::Mat output;
                    bool drowsy = detector.detect(detectInput, output);
            
                    QMetaObject::invokeMethod(this, [this, drowsy]() {
                        ui->label_fati->setText(drowsy ? "Fatigue Detected" : "Normal");
                    });
                    analyzing = false;
                });
            }
            else if (mode == SystemMode::Temp) {
                QMetaObject::invokeMethod(this, [this]() {
                    ui->label_video->setText("Mode is changing, please wait .....");
                });
                std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
            }
        }
    });
}

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

        int fps = 30;
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
        if (videoWriter.isOpened())
        {
            videoWriter.release();
        }
        ui->btn_record->setText("Start Recording");

        QMessageBox::information(this, "Recording Saved", "Video has been saved.");
    }
}
