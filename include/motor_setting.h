#ifndef _MOTOR_SETTING_H
#define _MOTOR_SETTING_H

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
    int step_delay = 900;

public:
    bool start(int chipNo, int pin1, int pin2, int pin3, int pin4);
    void forward(int steps);
    void backward(int steps);
    void cleanup();
    void step(int stepPattern[4]);
};

#endif
