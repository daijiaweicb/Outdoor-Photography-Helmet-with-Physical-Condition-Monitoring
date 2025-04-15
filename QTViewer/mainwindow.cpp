#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "libcam2opencv.h"
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>

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
    cv::Mat clone = frame.clone();
    QImage qimg(clone.data, clone.cols, clone.rows, clone.step, QImage::Format_RGB888);
    currentFrame = qimg.copy();
    ui->label_video->setPixmap(QPixmap::fromImage(currentFrame));

    if (isRecording) {
        cv::Mat bgr;
        cv::cvtColor(clone, bgr, cv::COLOR_RGB2BGR); 
        videoWriter.write(bgr);
    }
}

void MainWindow::on_btn_record_clicked()
{
    if (!isRecording) {
        QString filename = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".avi";
        int fps = 30;
        QSize size = currentFrame.size();

        videoWriter.open(filename.toStdString(),
                         cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                         fps,
                         cv::Size(size.width(), size.height()));

        if (!videoWriter.isOpened()) {
            qDebug() << "Failed to open video file for writing.";
            return;
        }

        isRecording = true;
        ui->btn_record->setText("Stop Recording");
    } else {
        isRecording = false;
        videoWriter.release();
        ui->btn_record->setText("Start Recording");
    }
}

