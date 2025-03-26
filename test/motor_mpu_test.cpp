#include <iostream>
// #include "motor_control.h"
#include "motor_setting.h"
#include "MPU6050.h"

using namespace std;
class MotorControl : public MPU, public StepperMotor, public MPUCallbackInterface
{
private:
    StepperMotor motor;
    struct Data //motor angle control data
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
    void MPUCallback(float& data)override;
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
int main()
{
    MPU mpu;
    MotorControl motorapp;

    mpu.RegisterSetting(&motorapp);
    mpu.beginMPU6050();

    return 0;
}
