#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "MG90S_setting.h"
#include "MPU6050.h"

// Just as a test, not for displaying in the QT interface

/**
 * @brief Callback function that processes real-time angle and temperature data from the MPU sensor.
 *
 * This method is intended to be invoked whenever new angle data (e.g., from an IMU or MPU6050) is received.
 * It updates internal angle tracking, computes delta changes, and sets the servo motor accordingly
 * based on the current `SystemMode`.
 *
 * Behavior by mode:
 * - Normal Mode: Adjusts servo angle dynamically based on pitch/roll data to compensate head movement.
 * - FatigueDetection Mode: Holds servo at center (90°) to remain stable during detection.
 *
 * @param data Struct containing real-time angle (roll) and temperature information from the MPU.
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
    void MPUCallback(AngleData &data) override;
    MotorControl()
    {
        angle.PrevData = 0;
    }
};

#endif
