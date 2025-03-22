#include <iostream>
#include "motor_control.h"
#include "MPU6050.h"

using namespace std;

int main()
{
    MPU mpu;
    MotorControl motorapp;

    mpu.RegisterSetting(&motorapp);
    mpu.beginMPU6050();

    return 0;
}
