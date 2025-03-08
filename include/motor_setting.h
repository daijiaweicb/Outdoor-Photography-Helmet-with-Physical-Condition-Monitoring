#ifndef _MOTOR_SETTING_H
#define _MOTOR_SETTING_H

#include <iostream>
#include <gpiod.h>
#include <unistd.h>
#include "MPU6050.h"
#include <mutex>

#define motor_pin1 17
#define motor_pin2 27
#define motor_pin3 22
#define motor_pin4 5

class StepperMotor
{
private:
    gpiod_chip *chipGPIO = nullptr;
    gpiod_line *pins[4] = {nullptr};
    int gpio_pins[4];
    int step_delay = 1000;

public:
    bool start();
    void forward(int steps);
    void backward(int steps);
    void cleanup();
    void step(int stepPattern[4]);
};

#endif
