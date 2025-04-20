#include "motor_thread.h"
#include <QDebug>

MotorThread::MotorThread(StepperMotor *injected_motor, QObject *parent)
    : QThread(parent)
{
    if (injected_motor)
    {
        motor = injected_motor;
        own_motor = false;
    }
    else
    {
        motor = new StepperMotor();
        own_motor = true;
    }
}

MotorThread::~MotorThread()
{
    if (own_motor && motor)
    {
        delete motor;
    }
}

void MotorThread::run()
{
    if (!motor->start(0, 17, 25, 27, 22))
    {
        qDebug() << "[MotorThread] Stepmotor init failed";
        return;
    }

    if (g_systemMode == SystemMode::Normal)
    {
        g_systemMode = SystemMode::Temp;
        emit modeChanged(g_systemMode);

        motor->forward(1900);
        motor->waitUntilDone();

        g_systemMode = SystemMode::FatigueDetection;
        emit modeChanged(g_systemMode);
    }
    else if (g_systemMode == SystemMode::FatigueDetection)
    {
        g_systemMode = SystemMode::Temp;
        emit modeChanged(g_systemMode);

        motor->backward(1900);
        motor->waitUntilDone();

        g_systemMode = SystemMode::Normal;
        emit modeChanged(g_systemMode);
    }

    emit modeChanged(g_systemMode);

    motor->cleanup(); 
}
