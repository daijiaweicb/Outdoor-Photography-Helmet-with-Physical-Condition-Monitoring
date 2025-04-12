#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include "Stepmotor_setting.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->ChangeMode, &QPushButton::clicked, this, &MainWindow::on_pushButton_rotate_clicked);

    // ui->VideoContainer->installEventFilter(this);

    // cap.open(0);
    // if (!cap.isOpened()) {
    //     qDebug("Cannot open camera");
    //     return;
    // }

    // timer = new QTimer(this);
    // connect(timer, &QTimer::timeout, this, &MainWindow::readFrame);
    // timer->start(60);
}
MainWindow::~MainWindow()
{
    // cap.release();
    delete ui;
}

void MainWindow::on_pushButton_rotate_clicked()
{
    StepperMotor motor;
    if (!motor.start(0, 17, 18, 27, 22)) {
        qDebug("Init stepmotor falied");
        return;
    }

    motor.forward(113);

    motor.cleanup();
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

