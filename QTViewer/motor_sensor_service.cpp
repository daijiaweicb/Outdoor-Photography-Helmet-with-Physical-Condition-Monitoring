#include "motor_sensor_service.h"

MotorSensorService::MotorSensorService(MG90S* sharedServo)
:   servo(sharedServo)
{}

void MotorSensorService::start()
{
    MPU& mpu = motorapp;
    mpu.RegisterSetting(&motorapp);
    mpu.beginMPU6050();
    mpu.startWorker();
}

void MotorSensorService::stop() //Used to expand rollout logic
{

}
