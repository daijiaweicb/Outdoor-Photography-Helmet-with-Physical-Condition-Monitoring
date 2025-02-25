#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "motor_setting.h"
#include "Timer.h"

class MotorControl : public SensorCallback, public StepperMotor
{
private:
    StepperMotor motor;
    std::mutex motor_mutex;
    struct Data
    {
        float PrevData;
        float NewData;
        float RevData;
    };
    Data angle;
    HighPrecisionTimer timer_1s;
    int count;
    bool time_flag;

public:
    MotorControl()
    {
        time_flag = 1;
        count = 0;
        angle.PrevData = 0;
        timer_1s.start(1000, [&]()
                       { time_flag = 1; });
    }
    void onSensorData(float value) override;
};

#endif