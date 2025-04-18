#ifndef MOTOR_CONTROL_QT_H
#define MOTOR_CONTROL_QT_H

#include "MG90S_setting.h"
#include "MPU6050.h"
#include <QObject>

// For QT inferface

/**
 * @class MotorControlQT
 * @brief Qt-enabled controller combining servo (MG90S) and sensor (MPU6050) feedback with real-time updates.
 *
 * This class serves as a bridge between the real-time sensor data (from MPU6050) and motor control (MG90S),
 * providing a Qt signal/slot interface for UI updates. It inherits:
 * - QObject: to emit signals to the UI
 * - MPU: to access sensor logic and register for callbacks
 * - MPU::MPUCallbackInterface: to receive sensor data via callback
 * - MG90S: to control the servo motor
 *
 * The class processes incoming roll angle data, calculates deltas, and updates servo position accordingly.
 */
class MotorControlQT : public QObject, public MPU, public MPU::MPUCallbackInterface, public MG90S
{
    Q_OBJECT

private:
    /**
     * @struct Data
     * @brief Internal structure for tracking and computing angle control values.
     */
    struct Data
    {
        float PrevData = 0.0f;     // Last angle reading
        float NewData = 0.0f;      // Latest angle reading
        float RevData = 0.0f;      // Difference between new and previous
        float target_angle = 0.0f; // Reserved for future PID implementation
    };
    Data angle;

public:
    /**
     * @brief Constructor with optional parent for Qt object hierarchy.
     * @param parent Pointer to parent QObject (default nullptr)
     */
    explicit MotorControlQT(QObject *parent = nullptr);

    /**
     * @brief Callback function triggered by MPU6050 data updates.
     *
     * Processes new angle and temperature data:
     * - Adjusts servo motor based on roll angle.
     * - Emits signals to UI for display.
     *
     * @param data Struct containing current roll angle and temperature.
     */
    void MPUCallback(AngleData &data) override;

Q_SIGNALS:
    /**
     * @brief Emitted whenever temperature updates from the MPU.
     * @param temp Current temperature in Celsius
     */
    void temperatureUpdated(float temp);

    /**
     * @brief Emitted when the latest roll angle is updated.
     * @param angle Roll angle in degrees
     */
    void angleUpdate(float temp);
};

#endif
