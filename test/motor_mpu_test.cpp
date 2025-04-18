#include <iostream>
#include "motor_control.h"
#include "MG90S_setting.h"
#include "MPU6050.h"

using namespace std;

int main()
{
    MG90S servo;
    MotorControl motorapp(&servo);
    servo.start_mg90s();

    MPU &mpu = motorapp;
    mpu.RegisterSetting(&motorapp);
    mpu.beginMPU6050();
    mpu.startWorker();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
