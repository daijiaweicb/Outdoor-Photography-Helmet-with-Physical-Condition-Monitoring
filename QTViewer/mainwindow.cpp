#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include "motor_thread.h"
#include "Mode.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // service = new MotorSensorService();
    // service->start();
    connect(ui->ChangeMode, &QPushButton::clicked, this, &MainWindow::on_ChangeMode_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ChangeMode_clicked()
{
    if (motorThread && motorThread->isRunning()) {
        qDebug() << "motor is running....Do not operate....";
        return;
    }

    motorThread = new MotorThread(this);

    connect(motorThread, &MotorThread::modeChanged,
            this, &MainWindow::onModeChanged);

    connect(motorThread, &MotorThread::finished, this, [this]() {
        motorThread->deleteLater();
        motorThread = nullptr;
    });

    motorThread->start();
}

void MainWindow::onModeChanged(SystemMode newMode)
{
    if (newMode == SystemMode::Normal)
        ui->label_status->setText("Current Mode：Normal");
    else if (newMode == SystemMode::FatigueDetection)
        ui->label_status->setText("Current Mode：FatigueDetection");
}

void MainWindow::on_Exit_clicked()
{
    auto reply = QMessageBox::question(this, "Exit Confirmation", "Are you sure you want to exit?",
                                       QMessageBox::No | QMessageBox::Yes);
    if (reply == QMessageBox::Yes) {
        QApplication::quit();
    }
}

// void MainWindow::readFrame()
// {
//     cv::Mat frame;
//     cap >> frame;
//     if (frame.empty()) return;

//     cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
//     currentFrame = QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888).copy();

//     ui->VideoContainer->update();
// }

// bool MainWindow::eventFilter(QObject *watched, QEvent *event)
// {
//     if (watched == ui->VideoContainer && event->type() == QEvent::Paint && !currentFrame.isNull()) {
//         QPainter painter(ui->VideoContainer);
//         painter.drawImage(ui->VideoContainer->rect(), currentFrame);
//         return true;
//     }
//     return QMainWindow::eventFilter(watched, event);
// }

