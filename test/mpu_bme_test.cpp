#include <iostream>
#include "motor_control.h"
#include "MPU6050.h"
#include "BME280.h"

class BMERealization : public BME, public BME::Callbackinterface
{
public:
    void BMECallback(BMEresults &results) override
    {
        std::cout << "temp is: " << results.temp << " pressure is: " << results.press << std::endl;
    }
};

int main()
{
    MPU mpu;
    MotorControl motorapp;
    BME bme;
    BMERealization bmeReal;

    mpu.RegisterSetting(&motorapp);
    bme.BMERigister(&bmeReal);
    
    mpu.beginMPU6050();
    bme.beginBME();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
