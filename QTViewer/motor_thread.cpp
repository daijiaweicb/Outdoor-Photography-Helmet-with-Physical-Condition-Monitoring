#include "motor_thread.h"
#include <QDebug>

MotorThread::MotorThread(QObject *parent)
    : QThread(parent)
{
}

MotorThread::~MotorThread()
{
}

void MotorThread::run()
{
    if (!motor.start(0, 17, 25, 27, 22)) {
        qDebug() << "Stepmotor init failed";
        return;
    }

    if (g_systemMode == SystemMode::Normal) {
        g_systemMode = SystemMode::Temp;
        motor.forward(2048);
        g_systemMode = SystemMode::FatigueDetection;
    } else if (g_systemMode == SystemMode::FatigueDetection) {
        g_systemMode = SystemMode::Temp;
        motor.backward(2048);
        servo.setAngle(90);
        QThread::sleep(1);
        g_systemMode = SystemMode::Normal;
    }

    emit modeChanged(g_systemMode);
    motor.cleanup();
}
