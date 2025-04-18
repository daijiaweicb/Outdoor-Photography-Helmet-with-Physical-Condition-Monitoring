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
/**
 * @brief Core logic of the thread, executed when thread is started.
 *
 * Based on the global `g_systemMode`, the function:
 * - Rotates the stepper motor forward if in `Normal` mode and transitions to `FatigueDetection`
 * - Rotates the motor backward if in `FatigueDetection` and transitions to `Normal`
 *
 * Emits modeChanged signals during transition and calls motor.cleanup() at the end.
 */
void MotorThread::run()
{
    // Initialize stepper motor with GPIO pins
    if (!motor->start(0, 17, 25, 27, 22))
    {
        qDebug() << "Stepmotor init failed";
        return;
    }
    // Handle mode transition
    if (g_systemMode == SystemMode::Normal)
    {
        g_systemMode = SystemMode::Temp;
        emit modeChanged(g_systemMode);
        motor->forward(1900); // Rotate to FatigueDetection position
        g_systemMode = SystemMode::FatigueDetection;
        emit modeChanged(g_systemMode);
    }
    else if (g_systemMode == SystemMode::FatigueDetection)
    {
        g_systemMode = SystemMode::Temp;
        emit modeChanged(g_systemMode);
        motor->backward(1900); // Rotate back to Normal position
        g_systemMode = SystemMode::Normal;
        emit modeChanged(g_systemMode);
    }

    emit modeChanged(g_systemMode);
    motor->cleanup();
}
