#include <iostream>
#include "motor.h"
#include "MPU6050.h"

using namespace std;

int main()
{
    StepperMotor motor;
    if (!motor.start(0, 17, 27, 22, 5)) 
    {
        cout << "Motor initialization failed!" << endl;
        return 1;
    }

    ThreadMPU threadm;
    threadm.start();

    threadm.stop();

    return 0;
}