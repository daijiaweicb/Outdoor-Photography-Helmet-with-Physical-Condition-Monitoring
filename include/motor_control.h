#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "motor_setting.h"

class MotorControl : public SensorCallback, public StepperMotor
{
private:
    StepperMotor motor;
    std::mutex motor_mutex;
    std::chrono::steady_clock::time_point last_action_time;
    std::chrono::milliseconds action_interval{100}; // 默认间隔 100ms
    struct Data
    {
        float PrevData;
        float NewData;
        float RevData;
    };
    Data angle;

public:
    MotorControl();
    void onSensorData(float value) override;
    void setActionInterval(int milliseconds);
};

#endif