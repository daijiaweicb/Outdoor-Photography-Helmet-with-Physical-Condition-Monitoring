#include "motor_control.h"
#include <cmath>

void MotorControl::MPUCallback(AngleData& data)
{
    angle.NewData = data.roll;
    angle.RevData = angle.NewData - angle.PrevData;

    std::cout << "Roll: " << angle.NewData << "°" << " Δ: " << angle.RevData
              << " Temp: " << data.temp << "℃" << std::endl;

    float mapped_angle = angle.NewData + 90;  // 把 [-90,90] -> [0,180]
    if (mapped_angle < 0) mapped_angle = 0;
    if (mapped_angle > 180) mapped_angle = 180;

    motor.setAngle(static_cast<int>(mapped_angle));

    angle.PrevData = angle.NewData;
}
