#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "MG90S_setting.h"
#include "MPU6050.h"

class MotorControl : public MPU, public MPU::MPUCallbackInterface, public MG90S 
{
private:
    MG90S motor;
    struct Data // motor angle control data
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
    void MPUCallback(AngleData &data) override;
    MotorControl()
    {
        angle.PrevData = 0;
    }
};

#endif
