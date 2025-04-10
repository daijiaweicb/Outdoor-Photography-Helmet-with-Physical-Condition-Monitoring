#include "pwm.h"

int PWM::start_pwm(int channel, int frequency, float duty_cycle, int chip)
{
    chippath = "/sys/class/pwm/pwmchip" + to_string(chip);
    pwmpath = chippath + "/pwm" + to_string(channel);
    string p = chippath + "/export";
    FILE *const fp = fopen(p.c_str(), "w");
    if (NULL == fp)
    {
        fprintf(stderr, "PWM device does not exist. Make sure to add 'dtoverlay=pwm-2chan' to /boot/firmware/config.txt.\n");
        return -1;
    }
    const int r = fprintf(fp, "%d", channel);
    if (r < 0)
        return r;
    usleep(100000);
    per = (int)1E9 / frequency;
    setPeriod(per);
    setDutyCycle(duty_cycle);
    enable();
    return r;
}
