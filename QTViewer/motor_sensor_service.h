#ifndef MOTOR_SENSOR_SERVICE_H
#define MOTOR_SENSOR_SERVICE_H

#include "motor_control_QT.h"

/**
 * @class MotorSensorService
 * @brief Encapsulates initialization and management of motor and sensor control components.
 *
 * This class provides a high-level interface to coordinate:
 * - Starting the servo motor (MG90S)
 * - Configuring and launching the MPU6050 sensor (via MotorControlQT)
 * - Accessing the control interface for real-time updates (angle, temperature)
 */
class MotorSensorService
{
public:
    /**
     * @brief Default constructor.
     */
    MotorSensorService();

    /**
     * @brief Starts the motor-sensor subsystem:
     * - Initializes the servo
     * - Configures MPU6050 and starts its worker thread
     */
    void start();

    /**
     * @brief Stops the motor-sensor subsystem (placeholder for cleanup or shutdown logic).
     */
    void stop();

    /**
     * @brief Returns a pointer to the MotorControlQT interface.
     * Used by UI or other modules to connect to sensor signals.
     */
    MotorControlQT *getMotorControl();

private:
    MG90S *servo = nullptr;  // Unused raw pointer to servo (can be removed)
    MotorControlQT motorapp; // Main motor-sensor controller (inherits both MG90S & MPU behavior)
};

#endif // MOTOR_SENSOR_SERVICE_H
