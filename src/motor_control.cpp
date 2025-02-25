#include "motor_control.h"

void MotorControl::onSensorData(float value)
{
    // std::lock_guard<std::mutex> lock(motor_mutex);

    if (time_flag == 1)
    {
        angle.NewData = value;
        angle.RevData = angle.NewData - angle.PrevData;
        angle.PrevData = angle.NewData;

        std::cout << "Angle Change: " << angle.RevData <<" New Angle: " << angle.NewData<< "°\n";
        time_flag = 0;
    }
}
