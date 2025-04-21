#include "motor_thread.h"
#include <QDebug>

std::mutex MotorThread::motorLock;

MotorThread::MotorThread(StepperMotor *injected_motor, QObject *parent)
    : QThread(parent)
    {
        if (injected_motor) {
            motor = injected_motor;
            own_motor = false;
        } else {
            motor = new StepperMotor();
            own_motor = true;
        }
    }
    
    MotorThread::~MotorThread()
    {
        if (own_motor && motor) {
            delete motor;
            motor = nullptr;
        }
    }
/**
 * @brief Entry point of the thread.
 *        Controls the stepper motor based on the current system mode.
 *        Uses a global lock to prevent concurrent motor access.
 */
void MotorThread::run()
{
    // Ensure exclusive access to the motor
    std::unique_lock<std::mutex> guard(motorLock);

    // Initialize the motor if not already initialized
    if (!motor->start(0, 17, 25, 27, 22)) {
        qDebug() << "[MotorThread] Stepmotor init failed";
        return;
    }

    // === Mode: Normal → FatigueDetection ===
    if (g_systemMode == SystemMode::Normal)
    {
        // Enter temporary state to prevent other operations
        g_systemMode = SystemMode::Temp;
        emit modeChanged(g_systemMode);

        // Move motor forward and wait for completion
        motor->forward(1900);
        qDebug() << "start forward";
        motor->waitUntilDone();
        qDebug() << "waitUntilDone finished";

        // Switch to fatigue detection mode
        g_systemMode = SystemMode::FatigueDetection;
        emit modeChanged(g_systemMode);
    }

    // === Mode: FatigueDetection → Normal ===
    else if (g_systemMode == SystemMode::FatigueDetection)
    {
        // Enter temporary state to prevent other operations
        g_systemMode = SystemMode::Temp;
        emit modeChanged(g_systemMode);

        // Move motor backward and wait for completion
        motor->backward(1900);
        motor->waitUntilDone();

        // Switch back to normal mode
        g_systemMode = SystemMode::Normal;
        emit modeChanged(g_systemMode);
    }

    // Emit the final mode to update UI or state machine
    emit modeChanged(g_systemMode);

    // Stop timer, release GPIO, and clean up resources
    motor->cleanup();
}
