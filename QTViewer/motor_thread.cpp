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
    servo.start_mg90s(); 
    if (!motor.start(0, 17, 25, 27, 22)) {
        qDebug() << "Stepmotor init failed";
        return;
    }

    if (g_systemMode == SystemMode::Normal) {
        g_systemMode = SystemMode::Temp;
        emit modeChanged(g_systemMode);
        motor.forward(2048);
        g_systemMode = SystemMode::FatigueDetection;
        emit modeChanged(g_systemMode);
    } else if (g_systemMode == SystemMode::FatigueDetection) {
        g_systemMode = SystemMode::Temp;
        emit modeChanged(g_systemMode);
        motor.backward(2048);
        servo.setAngle(90);
        g_systemMode = SystemMode::Normal;
        emit modeChanged(g_systemMode);
    }

    emit modeChanged(g_systemMode);
    motor.cleanup();
}
