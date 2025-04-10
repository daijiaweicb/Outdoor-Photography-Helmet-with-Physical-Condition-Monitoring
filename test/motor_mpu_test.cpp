#include <iostream>
#include "motor_control.h"
#include "MG90S_setting.h"
#include "MPU6050.h"

using namespace std;

int main()
{

    MotorControl motorapp;

    motorapp.start_mg90s();
    motorapp.beginMPU6050();
    motorapp.RegisterSetting(&motorapp);
    motorapp.startWorker();

    return 0;
}
