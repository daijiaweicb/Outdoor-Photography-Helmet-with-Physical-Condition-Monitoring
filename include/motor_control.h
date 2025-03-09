#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "motor_setting.h"
#include "MPU6050.h"

class MotorControl : public StepperMotor, public CallbackInterface
{
private:
    StepperMotor motor;
    std::mutex motor_mutex;
    struct Data
    {
        float PrevData;
        float NewData;
        float RevData;
        float target_angle;
        float integral;
        float prev_error;
    };
    Data angle;

public:
    void SensorCallback(float value) override;
    MotorControl()
    {
        angle.PrevData = 0;
        if (motor.start())
        {
            std::cout << "motor init success" << std::endl;
        }
    }

    ~MotorControl()
    {
        motor.cleanup();
    }
};

#endif