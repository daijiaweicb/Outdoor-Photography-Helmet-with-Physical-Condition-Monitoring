#include "Stepmotor_setting.h"
#include <iostream>
#include <algorithm>

using namespace std;

bool StepperMotor::start(int chipNo, int pin1, int pin2, int pin3, int pin4)
{
    chipGPIO = gpiod_chip_open_by_number(chipNo);
    if (!chipGPIO)
    {
        cout << "Failed to open GPIO chip" << endl;
        return false;
    }

    gpio_pins[0] = pin1;
    gpio_pins[1] = pin2;
    gpio_pins[2] = pin3;
    gpio_pins[3] = pin4;

    for (int i = 0; i < 4; i++)
    {
        pins[i] = gpiod_chip_get_line(chipGPIO, gpio_pins[i]);
        if (!pins[i])
        {
            cout << "Failed to get GPIO line " << gpio_pins[i] << endl;
            cleanup();
            return false;
        }

        if (gpiod_line_request_output(pins[i], "stepper_motor", 0) < 0)
        {
            cout << "Failed to set GPIO " << gpio_pins[i] << " as output" << endl;
            cleanup();
            return false;
        }
    }

    return true;
}

void StepperMotor::forward(int steps)
{
    goingForward = true;
    totalSteps = steps;
    stepCount = 0;
    currentStep = 0;

    {
        std::lock_guard<std::mutex> lock(cv_mutex);
        isBusy = true;
    }

    int ms = std::max(1, step_delay / 1000);
    timer.start(ms, [this]()
                { this->onStep(); });
}

void StepperMotor::backward(int steps)
{
    goingForward = false;
    totalSteps = steps;
    stepCount = 0;
    currentStep = 0;

    {
        std::lock_guard<std::mutex> lock(cv_mutex);
        isBusy = true;
    }

    int ms = std::max(1, step_delay / 1000);
    timer.start(ms, [this]()
                { this->onStep(); });
}

void StepperMotor::onStep()
{
    std::cout <<"onStep called" << std::endl;
    if (stepCount >= totalSteps)
    {
        std::thread([this]() {
            timer.stop();
        }).detach();
        {
            std::lock_guard<std::mutex> lock(cv_mutex);
            isBusy = false;
        }

        cv.notify_all();
        return;
    }

    static const int stepSequence[8][4] = {
        {1, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 1},
        {0, 0, 0, 1},
        {1, 0, 0, 1}};

    static const int stepSequenceReverse[8][4] = {
        {1, 0, 0, 1},
        {0, 0, 0, 1},
        {0, 0, 1, 1},
        {0, 0, 1, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {1, 1, 0, 0},
        {1, 0, 0, 0}};

    const int (*sequence)[4] = goingForward ? stepSequence : stepSequenceReverse;

    for (int i = 0; i < 4; i++)
    {
        gpiod_line_set_value(pins[i], sequence[currentStep][i]);
    }

    currentStep = (currentStep + 1) % 8;
    stepCount++;
}

void StepperMotor::cleanup()
{
    timer.stop();
    {
        std::lock_guard<std::mutex> lock(cv_mutex);
        isBusy = false;
    }

    cv.notify_all();

    for (int i = 0; i < 4; i++)
    {
        if (pins[i])
        {
            gpiod_line_set_value(pins[i], 0);
            gpiod_line_release(pins[i]);
        }
    }

    if (chipGPIO)
    {
        gpiod_chip_close(chipGPIO);
    }
}

bool StepperMotor::isRunning() const
{
    std::lock_guard<std::mutex> lock(cv_mutex);
    return isBusy;
}

void StepperMotor::waitUntilDone()
{
    std::unique_lock<std::mutex> lock(cv_mutex);
    cv.wait(lock, [this]() { return !isBusy; });
}