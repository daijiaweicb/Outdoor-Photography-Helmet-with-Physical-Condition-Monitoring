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
    cam->start();
}

MainWindow::~MainWindow()
{
    if (cam)
    {
        cam->stop();
        delete cam;
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
    static std::atomic<bool> busy = false;
    
    if (g_systemMode == SystemMode::FatigueDetection)
    {
        if (busy)
            return;
        busy = true;

        cv::Mat input = frame.clone();

        std::thread([this, input]()
                    {
        cv::Mat flipped,output;
        cv::flip(input, flipped, 0);
        bool drowsy = detector.detect(flipped, output);

        if (output.empty()) {
            busy = false;
            return;
        }

        QImage qimg(output.data, output.cols, output.rows, output.step, QImage::Format_RGB888);
        QImage safeFrame = qimg.copy();


        QMetaObject::invokeMethod(this, [this, safeFrame, drowsy,output]() {
            currentFrame = safeFrame;
            ui->label_video->setPixmap(QPixmap::fromImage(currentFrame));
            ui->label_fati->setText(drowsy ? "Fatigue Detected " : "Normal ");

            if (isRecording && videoWriter.isOpened()) {
                cv::Mat bgr;
                cv::cvtColor(output, bgr, cv::COLOR_RGB2BGR);
                videoWriter.write(bgr);
            }

            busy = false;
        }); })
            .detach();
    }
    else if(g_systemMode == SystemMode::Normal)
    {
        cv::Mat clone = frame.clone();
        QImage qimg(clone.data, clone.cols, clone.rows, clone.step, QImage::Format_RGB888);
        currentFrame = qimg.copy();
        ui->label_video->setPixmap(QPixmap::fromImage(currentFrame));
        if (isRecording && videoWriter.isOpened())
        {
            cv::Mat bgr;
            cv::cvtColor(frame, bgr, cv::COLOR_RGB2BGR);
            videoWriter.write(bgr);
        }
    }
    else if(g_systemMode == SystemMode::Temp)
    {
        ui->label_video->setText("Mode is changing, please wait .....");
    }
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
