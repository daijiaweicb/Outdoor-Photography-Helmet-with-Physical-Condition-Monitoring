#include "motor_control.h"
#include <cmath>
#include "Mode.h"

void MotorControl::MPUCallback(AngleData &data)
{
    if(g_systemMode == SystemMode::Normal)
    {
        angle.NewData = data.roll;

    angle.RevData = angle.NewData - angle.PrevData;

    std::cout << "Angle Change: " << angle.RevData << " New Angle: " << angle.NewData << "°" << " Temp: " << data.temp << "℃" << std::endl;
    setAngle(90 - angle.NewData);

    angle.PrevData = angle.NewData;
    }
}
