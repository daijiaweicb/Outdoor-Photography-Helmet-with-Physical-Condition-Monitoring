#include "motor_control.h"
#include <cmath>

// void MotorControl::onSensorData(float value)
// {
//     std::lock_guard<std::mutex> lock(motor_mutex);
//     angle.NewData = value;

//     if (time_flag == 1)
//     {
//         // angle.target_angle = 90.0;

//         // angle.target_angle = external_target;

//         const float Kp = 0.1, Ki = 0.01, Kd = 0.2;
//         const float max_integral = 100.0;

//         float error = angle.target_angle - angle.NewData;

//         angle.integral = std::clamp(angle.integral + error, -max_integral, max_integral);

//         float derivative = error - angle.prev_error;
//         angle.prev_error = error;
//         float output = Kp * error + Ki * angle.integral + Kd * derivative;
//         std::cout << "Output: " << output << " New Angle: " << angle.NewData << "°\n";
//         if (fabs(output) > 1)
//         {
//             int intSteps = static_cast<int>(round(output / 0.08789));
//             if (intSteps > 0)
//             {
//                 motor.backward(intSteps);
//             }
//             else if (intSteps < 0)
//             {
//                 motor.forward(-intSteps);
//             }
//         }

//         time_flag = 0;
//     }

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
        motor.backward(intSteps);
    }
    else if (intSteps < -10)
    {
        motor.forward(-intSteps);
    }
    
    angle.PrevData = angle.NewData;
}
