#ifndef MG90S_SETTING_H
#define MG90S_SETTING_H

#include "pwm.h"

class MG90S
{
    public:
    
    void start_mg90s()
    {
        pwm.start_pwm(2,50,7.5,2);
    }

    void setAngle(int angle)
    {
        int duty_ns = angleToDutyNs(angle);

        pwm.setDutyCycleNS(duty_ns);
    }

    private:
    PWM pwm;

    int angleToDutyNs(int angle)
    {
        if(angle < 0)
        {
            angle = 0;
        }
        else if(angle > 180)
        {
            angle = 180;
        }
        return 500000 + angle * (2000000/180);
    }
};

#endif