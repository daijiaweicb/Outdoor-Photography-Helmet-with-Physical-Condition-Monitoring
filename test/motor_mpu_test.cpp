#include <iostream>
#include "Thread_MPU.h"
#include "motor_control.h"
#include "iic.h"
#include <vector>
#include <chrono>

using namespace std;

int main()
{
    IIC iic(1);
    iic.iic_open();

    StepperMotor motor;
    if (motor.start(0, 17, 27, 22, 5))
    {
        cout << "motor init success" << endl;
    }

    ThreadMPU threadm(iic, 0.01f);
    threadm.calibrate();
    std::cout << "Calibration done." << std::endl;

    std::cout << "Waiting for sensor stabilization..." << std::endl;

    MotorControl motorc;
    threadm.RegisterCallback(&motorc);
    std::cout << "Starting data acquisition..." << std::endl;
    threadm.start();

    std::cout << "Press ENTER to exit..." << std::endl;
    std::cin.get();

    threadm.stop();
    return 0;
}
