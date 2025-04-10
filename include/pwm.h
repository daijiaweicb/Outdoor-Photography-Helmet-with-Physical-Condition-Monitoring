#ifndef PWM_H
#define PWM_H

#include <iostream>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

// Reference: https://github.com/berndporr/rpi_pwm

/**
 * @brief PWM control class, based on the sysfs interface to control the PWM output on the Raspberry Pi.
 *
 * This class encapsulates writing to /sys/class/pwm/pwmchipX/pwmY,
 * including setting period, duty cycle, and enabling/disabling output.
 */
class PWM
{
public:
    /**
     * @brief Start the PWM output
     *
     * @param channel PWM channel (e.g., 2 for pwm2)
     * @param frequency Frequency in Hz (e.g., 50Hz for servo motors)
     * @param duty_cycle Duty cycle in percentage (0.0 ~ 100.0)
     * @param chip PWM chip number (usually 0 or 2)
     * @return int 0 if success, <0 if failed
     */
    int start_pwm(int channel, int frequency, float duty_cycle, int chip = 2);

    /**
     * @brief Set duty cycle by percentage
     *
     * @param v Duty cycle percentage (e.g., 7.5)
     * @return int Result code
     */
    inline int setDutyCycle(float v) const
    {
        const int dc = (int)round((float)per * (v / 100.0));
        const int r = setDutyCycleNS(dc);
        return r;
    }

    /**
     * @brief Set duty cycle in nanoseconds
     */
    inline int setDutyCycleNS(int ns) const
    {
        const int r = writeSYS(pwmpath + "/duty_cycle", ns);
        return r;
    }

    ~PWM()
    {
        disable(); // Disable PWM on object destruction
    }

private:
    string chippath; // Path to pwmchip (e.g., /sys/class/pwm/pwmchip2)
    string pwmpath;  // Path to specific pwm channel (e.g., /pwm2)
    int per = 0;     // Period in nanoseconds

    /**
     * @brief Write an integer value to a sysfs file
     */
    inline int writeSYS(std::string filename, int value) const
    {
        FILE *const fp = fopen(filename.c_str(), "w");
        if (NULL == fp)
        {
            return -1;
        }
        const int r = fprintf(fp, "%d", value);
        fclose(fp);
        return r;
    }

    /**
     * @brief Set PWM period in nanoseconds
     */
    void setPeriod(int ns) const
    {
        writeSYS(pwmpath + "/period", ns);
    }

    

    /**
     * @brief Enable PWM output
     */
    void enable() const
    {
        int r = writeSYS(pwmpath + "/enable", 1);
        cout << "[DEBUG] enable() write to " << pwmpath + "/enable" << " result = " << r << endl;
    }

    /**
     * @brief Disable PWM output
     */
    void disable() const
    {
        writeSYS(pwmpath + "/enable", 0);
    }
};

#endif
