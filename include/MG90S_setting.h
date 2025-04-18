#ifndef MG90S_SETTING_H
#define MG90S_SETTING_H

#include "pwm.h"
#include "IServoControl.h"

/**
 * @class MG90S
 * @brief Concrete implementation of IServoControl for MG90S servo motor.
 *
 * This class provides an abstraction over low-level PWM control logic
 * to drive an MG90S hobby servo motor. It implements the `IServoControl`
 * interface.
 *
 * Internally, it maps a desired angle (in degrees) to a corresponding
 * PWM duty cycle (in nanoseconds), and applies it via a `PWM` driver.
 */
class MG90S: public IServoControl
{
public:
    /**
     * @brief Initializes and starts the PWM signal for MG90S servo.
     *
     * The PWM is configured with:
     * - Channel: 2
     * - Frequency: 50 Hz (standard for hobby servos)
     * - Initial Duty Cycle: 7.5% (corresponding to ~90 degrees)
     * - Pin Group: 2
     */
    void start_mg90s()
    {
        pwm.start_pwm(2, 50, 7.5, 2);
    }

    /**
     * @brief Sets the servo to a specified angle.
     *
     * @param angle Desired angle in degrees (0–180).
     * Values below 0 are clamped to 0, values above 180 are clamped to 180.
     * Internally, the method maps the angle to a PWM duty cycle and applies it.
     */
    void setAngle(int angle)
    {
        int duty_ns = angleToDutyNs(angle);

        pwm.setDutyCycleNS(duty_ns);
    }

private:
    PWM pwm;
    /**
     * @brief Converts angle in degrees to PWM duty cycle in nanoseconds.
     *
     * @param angle Angle in degrees.
     * @return Corresponding duty cycle in nanoseconds.
     * The duty cycle typically ranges from 0.5 ms (0°) to 2.5 ms (180°),
     * i.e., 500,000 ns to 2,500,000 ns.
     */
    int angleToDutyNs(int angle)
    {
        if (angle < 0)
        {
            angle = 0;
        }
        else if (angle > 180)
        {
            angle = 180;
        }
        return 500000 + angle * (2000000 / 180);
    }
};

#endif
