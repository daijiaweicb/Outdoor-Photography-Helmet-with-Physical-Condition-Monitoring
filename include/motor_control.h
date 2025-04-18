#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "MG90S_setting.h"
#include "MPU6050.h"

// Just as a test, not for displaying in the QT interface

/**
 * @class MotorControl
 * @brief High-level controller for servo positioning based on MPU6050 sensor input.
 *
 * This class processes real-time orientation data (roll, temp) from an MPU6050 sensor,
 * and adjusts a connected servo motor (via the IServoControl interface) accordingly.
 *
 * It inherits from:
 * - `MPU`: to access MPU6050 initialization and data acquisition.
 * - `MPUCallbackInterface`: to receive callbacks when new sensor data is available.
 *
 * The servo motor dependency is injected via the constructor, enabling decoupling from
 * hardware-specific implementations like `MG90S` and facilitating unit testing.
 */
class MotorControl : public MPU, public MPU::MPUCallbackInterface, public MG90S
{
private:
    struct Data // motor angle control data
    {
        float PrevData;
        float NewData;
        float RevData;
        float target_angle;
        float integral;
        float prev_error;
    };
    Data angle;

public:
    void MPUCallback(const AngleData &data) override;
    explicit MotorControl(IServoControl* servo) : servo_(servo) {}

protected:
    IServoControl *servo_ = nullptr; // Injected servo control object
};

#endif
