#include "motor_control_QT.h"
#include <cmath>
#include "Mode.h"

/**
 * @brief Constructor. Initializes internal angle values.
 */
MotorControlQT::MotorControlQT(QObject *parent)
    : QObject(parent)
{
    angle.PrevData = 0;
}

/**
 * @brief Callback executed when MPU6050 provides new data.
 *
 * - Updates internal angle values.
 * - Computes delta between new and previous readings.
 * - Applies real-time motor control:
 *     - In Normal mode: adjusts servo proportionally to roll angle.
 *     - In FatigueDetection mode: locks servo to center (90°).
 * - Emits temperature and angle signals for UI update.
 *
 * @param data Reference to AngleData containing roll and temp.
 */
void MotorControlQT::MPUCallback(const AngleData &data)
{

    angle.NewData = data.roll;

    angle.RevData = angle.NewData - angle.PrevData;

    // std::cout << "Angle Change: " << angle.RevData << " New Angle: " << angle.NewData << "°" << " Temp: " << data.temp << "℃" << std::endl;

    // Mode-dependent servo behavior
    if (g_systemMode == SystemMode::Normal)
    {
        setAngle(90 + angle.NewData); // compensate head tilt
    }
    else if (g_systemMode == SystemMode::FatigueDetection)
    {
        setAngle(90); // lock center for stable detection
    }

    // Emit data to UI
    Q_EMIT temperatureUpdated(data.temp);
    Q_EMIT angleUpdate(angle.NewData);

    // Save current angle for next comparison
    angle.PrevData = angle.NewData;
}
