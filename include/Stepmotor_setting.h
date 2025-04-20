#ifndef _STEPMOTOR_SETTING_H
#define _STEPMOTOR_SETTING_H

#include <iostream>
#include <gpiod.h>
#include <unistd.h>
#include "Timer.h"

class StepperMotor
{
private:
    gpiod_chip *chipGPIO = nullptr;
    gpiod_line *pins[4] = {nullptr};
    int gpio_pins[4];
    int step_delay = 1000;

    HighPrecisionTimer timer;
    int currentStep = 0;
    int stepCount = 0;
    int totalSteps = 0;
    bool goingForward = true;

public:
    virtual bool start(int chipNo, int pin1, int pin2, int pin3, int pin4);
    virtual void forward(int steps);
    virtual void backward(int steps);
    virtual void cleanup();

    void setStepDelay(int microseconds) { step_delay = microseconds; }

private:
    void step(int stepPattern[4]);
    void onStep();
};

#endif