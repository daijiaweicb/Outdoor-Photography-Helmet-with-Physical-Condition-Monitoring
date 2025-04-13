#include "motor_sensor_service.h"

MotorSensorService::MotorSensorService() {}

void MotorSensorService::start()
{
    MG90S& motor = motorapp;
    motor.start_mg90s();

    MPU& mpu = motorapp;
    mpu.RegisterSetting(&motorapp);
    mpu.beginMPU6050();
    mpu.startWorker();
}

void MotorSensorService::stop() //Used to expand rollout logic
{

}
