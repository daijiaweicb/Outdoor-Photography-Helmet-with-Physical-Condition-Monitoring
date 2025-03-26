#include <iostream>
#include "motor_control.h"
#include "motor_setting.h"
#include "MPU6050.h"

using namespace std;

int main()
{
    MPU mpu;
    StepperMotor motor;
    MotorControl motorapp;

    motor.start();
    mpu.RegisterSetting(&motorapp);
    mpu.beginMPU6050();

    return 0;
}
