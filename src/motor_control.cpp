#include "motor_control.h"
#include <cmath>
#include "Mode.h"

void MotorControl::MPUCallback(AngleData &data)
{

    angle.NewData = data.roll;

    angle.RevData = angle.NewData - angle.PrevData;

    std::cout << "Angle Change: " << angle.RevData << " New Angle: " << angle.NewData << "°" << " Temp: " << data.temp << "℃" << std::endl;
    
    if (g_systemMode == SystemMode::Normal)
    {
        setAngle(90 - angle.NewData);
    }

    angle.PrevData = angle.NewData;
}
