#include "motor_control.h"
#include <cmath>

void MotorControl::onSensorData(float value)
{
    // std::lock_guard<std::mutex> lock(motor_mutex);
    angle.NewData = value;
    if (time_flag == 1)
    {
        
        angle.RevData = angle.NewData - angle.PrevData;
        angle.PrevData = angle.NewData;

        std::cout << "Angle Change: " << angle.RevData <<" New Angle: " << angle.NewData<< "°\n";

        float steps = angle.RevData /0.08789;
        int intSteps = static_cast<int>(round(steps));


        
        if(intSteps > 0)
        {
            motor.forward(intSteps);
        }
        else if(intSteps < 0)
        {
            motor.backward(-intSteps);
        }

        time_flag = 0;
    }
}
