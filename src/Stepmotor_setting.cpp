#include "Stepmotor_setting.h"
#include <iostream>
#include <algorithm>

using namespace std;

/**
 * @brief Initializes GPIO chip and requests output lines for stepper motor control.
 */
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

    // Request output access to each GPIO pin
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

/**
 * @brief Starts the motor moving forward for a given number of steps.
 */
void StepperMotor::forward(int steps)
{
    stopped = false;
    goingForward = true;
    shouldStop = false;
    totalSteps = steps;
    stepCount = 0;
    currentStep = 0;

    {
        std::lock_guard<std::mutex> lock(cv_mutex);
        isBusy = true;
    }

    int ms = std::max(1, step_delay / 1000); // Convert µs to ms (minimum 1 ms)
    timer.start(ms, [this]() { this->onStep(); });
}

/**
 * @brief Starts the motor moving backward for a given number of steps.
 */
void StepperMotor::backward(int steps)
{
    stopped = false;
    goingForward = false;
    shouldStop = false;
    totalSteps = steps;
    stepCount = 0;
    currentStep = 0;

    {
        std::lock_guard<std::mutex> lock(cv_mutex);
        isBusy = true;
    }

    int ms = std::max(1, step_delay / 1000); // Convert µs to ms (minimum 1 ms)
    timer.start(ms, [this]() { this->onStep(); });
}

/**
 * @brief Called on each timer tick. Advances one step and stops when done.
 */
void StepperMotor::onStep()
{
    if (stepCount >= totalSteps)
    {
        if (!stopped.exchange(true))
        {
            // Stop timer in a separate thread to avoid joining within itself
            std::thread([this]() { this->timer.stop(); }).detach();
        }

        {
            std::lock_guard<std::mutex> lock(cv_mutex);
            isBusy = false;
            shouldStop = true;
            cv.notify_all();
            return;
        }
    }

    // 8-step full stepping sequence (half-step mode)
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

    // Write one step to GPIO
    for (int i = 0; i < 4; i++)
    {
        gpiod_line_set_value(pins[i], sequence[currentStep][i]);
    }

    currentStep = (currentStep + 1) % 8;
    stepCount++;

    if (shouldStop) return;
}

/**
 * @brief Stops the motor and releases all GPIO lines.
 */
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
            gpiod_line_set_value(pins[i], 0); // Set low before release
            gpiod_line_release(pins[i]);
        }
    }

    if (chipGPIO)
    {
        gpiod_chip_close(chipGPIO);
    }
}

/**
 * @brief Checks if the motor is currently active.
 */
bool StepperMotor::isRunning() const
{
    std::lock_guard<std::mutex> lock(cv_mutex);
    return isBusy;
}

/**
 * @brief Blocks until the motor has finished moving.
 */
void StepperMotor::waitUntilDone()
{
    std::unique_lock<std::mutex> lock(cv_mutex);
    cv.wait(lock, [this]() { return !isBusy; });

    if (!stopped)
    {
        timer.stop();
        stopped = true;
    }
}