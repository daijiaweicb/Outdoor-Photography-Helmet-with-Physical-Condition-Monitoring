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
 * the stepper motor accordingly. It uses a background thread to ensure non-blocking GUI behavior.
 */
class MotorThread : public QThread
{
    Q_OBJECT

public:
    explicit MotorThread(StepperMotor *injected_motor = nullptr, QObject *parent = nullptr);
    ~MotorThread() override;

    /**
     * @brief Entry point for the thread execution.
     *
     * Determines the current system mode and triggers the appropriate motor action.
     * Waits for the motor to finish (via condition_variable) before continuing.
     */
    void run() override;

Q_SIGNALS:
    /**
     * @brief Emitted whenever the system mode changes.
     * @param newMode The updated system mode
     */
    void modeChanged(SystemMode newMode);

protected:
    StepperMotor *motor = nullptr;
    bool own_motor = true;
};

#endif // MOTOR_THREAD_H
