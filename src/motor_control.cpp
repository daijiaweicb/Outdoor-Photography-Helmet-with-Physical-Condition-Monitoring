#include "motor_control.h"
#include <cmath>

void MotorControl::onSensorData(float value)
{




    // std::lock_guard<std::mutex> lock(motor_mutex);
    angle.NewData = value;
    if (time_flag == 1)
    {
        

    target_angle += (value - angle.PrevData);
 
 
        
    angle.NewData = value;
    float error = target_angle - angle.NewData;
    integral += error;
    float derivative = error - prev_error;
    prev_error = error;
    float output = Kp * error + Ki * integral + Kd * derivative;

    std::cout << "PID Output: " << output
    << " Current Angle: " << angle.NewData
    << " Target: " << target_angle << "°\n";


    int intSteps = static_cast<int>(round(output / 0.08789));

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
