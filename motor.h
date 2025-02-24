#ifndef _MOTOR_H
#define _MOTOR_H

#include <iostream>
#include <gpiod.h>
#include <unistd.h>  
#include "MPU6050.h"
#include <mutex>

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
    StepperMotor motor;
    std::mutex motor_mutex;
    struct Data
    {
        float PrevData;
        float NewData ;
        float RevData ;
    };
    Data angle;
    
    public:
    MotorControl();
    void onSensorData(float value) override;
};

#endif
