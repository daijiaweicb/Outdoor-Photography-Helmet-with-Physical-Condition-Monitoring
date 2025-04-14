#include "motor_control_QT.h"
#include <cmath>
#include "Mode.h"

MotorControlQT::MotorControlQT(QObject *parent)
    : QObject(parent)
{
    angle.PrevData = 0;
}

void MotorControlQT::MPUCallback(AngleData &data)
{

    angle.NewData = data.roll;

    angle.RevData = angle.NewData - angle.PrevData;

    std::cout << "Angle Change: " << angle.RevData << " New Angle: " << angle.NewData << "°" << " Temp: " << data.temp << "℃" << std::endl;

    if (g_systemMode == SystemMode::Normal)
    {
        setAngle(90 - angle.NewData);
    }
    else if (g_systemMode == SystemMode::FatigueDetection)
    {
        setAngle(90);
    }
    Q_EMIT temperatureUpdated(data.temp);
    Q_EMIT angleUpdate(angle.NewData);
    angle.PrevData = angle.NewData;
}
