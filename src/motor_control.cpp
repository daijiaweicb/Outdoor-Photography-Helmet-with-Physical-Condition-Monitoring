#include "motor_control.h"
#include <cmath>

void MotorControl::MPUCallback(float& data)
{
    // std::lock_guard<std::mutex> lock(motor_mutex);
    angle.NewData = data;

    angle.RevData = angle.NewData - angle.PrevData;

    // std::cout << "Angle Change: " << angle.RevData << " New Angle: " << angle.NewData << "°" << " tempeature is: " << data.temp << std::endl;
    std::cout << "Angle Change: " << angle.RevData << " New Angle: " << angle.NewData << "°" <<std::endl;
    float steps = angle.RevData / 0.08789;
    int intSteps = static_cast<int>(round(steps));

    // Motor Control
    if (intSteps > 10)
    {
        motor.forward(intSteps);
    }
    else if (intSteps < -10)
    {
        motor.backward(-intSteps);
    }
    
    angle.PrevData = angle.NewData;
}
