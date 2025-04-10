#include "motor_control.h"
#include <cmath>

void MotorControl::MPUCallback(AngleData& data)
{
    // std::lock_guard<std::mutex> lock(motor_mutex);
    angle.NewData = data.roll;

    angle.RevData = angle.NewData - angle.PrevData;

    // std::cout << "Angle Change: " << angle.RevData << " New Angle: " << angle.NewData << "°" << " tempeature is: " << data.temp << std::endl;
    std::cout << "Angle Change: " << angle.RevData << " New Angle: " << angle.NewData << "°" << " Temp: " << data.temp << "℃" <<std::endl;
    if(angle.RevData >= 90)
    {
        motor.setAngle(angle.RevData - 90);
    }
    else if(angle.RevData < 90)
    {
        motor.setAngle(angle.RevData);
    }

    
    angle.PrevData = angle.NewData;
}
