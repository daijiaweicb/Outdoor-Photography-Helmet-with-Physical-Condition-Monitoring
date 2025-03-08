#include <iostream>
#include "motor_control.h"
#include "MPU6050.h"
#include "iic.h"
#include <vector>
#include <chrono>

using namespace std;

int main()
{
    auto myCallback = std::make_shared<MotorControl>();
    MPU mpu;
    

    mpu.RegisterSetting(myCallback);
    mpu.beginMPU6050();

    return 0;
}
