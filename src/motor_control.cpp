#include "motor_control.h"
#include <cmath>

void MotorControl::onSensorData(float value)
{




    // std::lock_guard<std::mutex> lock(motor_mutex);
    angle.NewData = value;
    if (time_flag == 1)
    {
        

    angle.target_angle += (angle.NewData - angle.PrevData);
 
 
    float Kp = 2.0, Ki = 0.1, Kd = 0.5;  
    
    float error = angle.target_angle - angle.NewData;
    angle.integral += error;
    float derivative = error - angle.prev_error;
    angle.prev_error = error;
    float output = Kp * error + Ki * angle.integral + Kd * derivative;

    std::cout << 
    " Current Angle: " << angle.NewData
    << " Target: " << angle.target_angle << "°\n";


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
