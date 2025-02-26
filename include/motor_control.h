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
        float target_angle;
        float integral;
        float prev_error;
         
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
        timer_1s.start(200, [&]()
                       { time_flag = 1; });
        if (motor.start(0, 17, 27, 22, 5))
        {
            std::cout << "motor init success" << std::endl;
        }
    }

    ~MotorControl()
    {
        timer_1s.stop();
        motor.cleanup();
    }
    void onSensorData(float value) override;
};

#endif