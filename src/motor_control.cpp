#include "motor_control.h"
#include <cmath>

void MotorControl::SensorCallback(float value)
{
    // std::lock_guard<std::mutex> lock(motor_mutex);
    angle.NewData = value;

    angle.RevData = angle.NewData - angle.PrevData;

    std::cout << "Angle Change: " << angle.RevData << " New Angle: " << angle.NewData << "°\n";

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
