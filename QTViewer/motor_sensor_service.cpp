#include "motor_sensor_service.h"

/**
 * @brief Constructs the service manager. No setup is done here.
 */
MotorSensorService::MotorSensorService() {}

/**
 * @brief Starts the MG90S servo and configures the MPU6050 sensor.
 *
 * - Initializes the PWM output for the servo motor.
 * - Registers the motor controller as callback target for MPU updates.
 * - Starts background thread to poll sensor data.
 */
void MotorSensorService::start()
{
    // Start MG90S servo via MotorControlQT (inherits from MG90S)
    MG90S& motor = motorapp;
    motor.start_mg90s();

    // Set up MPU6050 sensor and pass MotorControlQT as the callback receiver
    MPU& mpu = motorapp;
    mpu.RegisterSetting(&motorapp);
    mpu.beginMPU6050();
    mpu.startWorker();
}

/**
 * @brief Provides access to the motor controller object.
 * Used externally to connect UI to signals (e.g., angleUpdated, temperatureUpdated).
 */
MotorControlQT* MotorSensorService::getMotorControl()
{
    return &motorapp;
}

/**
 * @brief Placeholder for stopping or cleaning up motor-sensor components.
 *
 * Currently unused but can be extended to:
 * - Stop MPU polling thread
 * - Reset servo position
 * - Free hardware resources
 */
void MotorSensorService::stop()
{

}
