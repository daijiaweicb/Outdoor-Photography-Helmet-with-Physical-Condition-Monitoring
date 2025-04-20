#ifndef MOTOR_THREAD_H
#define MOTOR_THREAD_H

#include <QThread>
#include "Stepmotor_setting.h"
#include "MG90S_setting.h"
#include "Mode.h"

/**
 * @class MotorThread
 * @brief A background thread responsible for controlling the stepper motor based on current system mode.
 *
 * This class transitions the system between `Normal` and `FatigueDetection` modes by rotating
 * the stepper motor accordingly. The thread is executed asynchronously to avoid blocking the main GUI thread.
 */
class MotorThread : public QThread
{
    Q_OBJECT
public:
    MotorThread(StepperMotor *injected_motor = nullptr, QObject *parent = nullptr);
    ~MotorThread();

    /**
     * @brief Main function of the thread.
     *
     * Determines current mode and executes the corresponding motor command (forward/backward).
     * Notifies mode changes via signal.
     */
    void run() override;

Q_SIGNALS:
    /**
     * @brief Emitted whenever the mode changes.
     * @param newMode The updated system mode
     */
    void modeChanged(SystemMode newMode);

protected:
    StepperMotor *motor = nullptr;
    bool own_motor = true;

private:
    void waitForMotor(); // Optional: helper function to wait until motor finishes
};

#endif // MOTOR_THREAD_H