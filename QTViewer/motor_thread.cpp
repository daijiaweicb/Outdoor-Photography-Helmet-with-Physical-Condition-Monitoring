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
    if (!motor.start(0, 17, 18, 27, 22)) {
        qDebug() << "Stepmotor init failed";
        return;
    }

    if (g_systemMode == SystemMode::Normal) {
        motor.forward(113);
        g_systemMode = SystemMode::FatigueDetection;
    } else if (g_systemMode == SystemMode::FatigueDetection) {
        motor.backward(113);
        g_systemMode = SystemMode::Normal;
    }

    emit modeChanged(g_systemMode);
    motor.cleanup();
}
