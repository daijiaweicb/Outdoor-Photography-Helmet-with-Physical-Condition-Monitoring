#include <iostream>
#include "motor_control.h"
#include "MG90S_setting.h"
#include "MPU6050.h"

using namespace std;

int main()
{
    MPU mpu;
    MG90S motor;
    MotorControl motorapp;

    motor.start_mg90s();
    mpu.RegisterSetting(&motorapp);
    mpu.beginMPU6050();

    return 0;
}
