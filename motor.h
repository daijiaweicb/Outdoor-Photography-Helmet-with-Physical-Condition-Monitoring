#ifndef _MOTOR_H
#define _MOTOR_H

#include <iostream>
#include <gpiod.h>
#include <unistd.h>  
#include "MPU6050.h"

class StepperMotor
{
private:
    gpiod_chip *chipGPIO = nullptr;
    gpiod_line *pins[4] = {nullptr}; 
    int gpio_pins[4]; 
    int step_delay = 1000; 

public:
    bool start(int chipNo, int pin1, int pin2, int pin3, int pin4);
    void forward(int steps);
    void backward(int steps);
    void cleanup();
    void step(int stepPattern[4]);
};



class MotorControl : public SensorCallback , public StepperMotor
{
    private:
    struct Data
    {
        float PrevData =0;
        float NewData =0;
        float RevData =0;
    };
    
    public:
    void onSensorData(float value) override;
};

#endif
