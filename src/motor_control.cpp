#include "motor_control.h"
#include <cmath>
#include "Mode.h"

void MotorControl::MPUCallback(AngleData &data)
{
    // Store the latest angle value
    angle.NewData = data.roll;

    // Compute difference from previous angle
    angle.RevData = angle.NewData - angle.PrevData;

    // Log angle change and temperature
    std::cout << "Angle Change: " << angle.RevData << " New Angle: " << angle.NewData << "°" << " Temp: " << data.temp << "℃" << std::endl;

    // Mode-dependent behavior
    if (g_systemMode == SystemMode::Normal)
    {
        // Adjust servo to compensate head tilt (e.g., stabilize head-mounted camera)
        setAngle(90 - angle.NewData);
    }
    else if (g_systemMode == SystemMode::FatigueDetection)
    {
        // Lock servo to center (90°) for consistent detection
        setAngle(90);
    }
    // Store current angle for next delta computation
    angle.PrevData = angle.NewData;
}
