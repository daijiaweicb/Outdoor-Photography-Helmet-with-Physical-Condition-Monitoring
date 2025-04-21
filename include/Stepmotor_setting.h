#ifndef _STEPMOTOR_SETTING_H
#define _STEPMOTOR_SETTING_H

#include <iostream>
#include <gpiod.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include "Timer.h"

/**
 * @brief StepperMotor class for controlling a 4-pin stepper motor
 *        using Linux GPIO (via libgpiod) and a high-precision timer.
 */
class StepperMotor
{
public:
    /**
     * @brief Initializes GPIO pins for motor control.
     * 
     * @param chipNo GPIO chip number (e.g., 0 for /dev/gpiochip0)
     * @param pin1-pin4 GPIO pin numbers connected to the motor coils
     * @return true if successful, false otherwise
     */
    virtual bool start(int chipNo, int pin1, int pin2, int pin3, int pin4);

    /**
     * @brief Starts motor rotation in forward direction.
     * 
     * @param steps Number of steps to move
     */
    virtual void forward(int steps);

    /**
     * @brief Starts motor rotation in backward direction.
     * 
     * @param steps Number of steps to move
     */
    virtual void backward(int steps);

    /**
     * @brief Releases all GPIO lines and stops the motor.
     */
    virtual void cleanup();

    /**
     * @brief Sets delay between steps in microseconds.
     *        A smaller value makes the motor rotate faster.
     */
    void setStepDelay(int microseconds) { step_delay = microseconds; }

    /**
     * @brief Returns whether the motor is currently running.
     */
    bool isRunning() const;

    /**
     * @brief Blocks the caller until the motor finishes its movement.
     */
    void waitUntilDone();

private:
    // GPIO control
    gpiod_chip *chipGPIO = nullptr;         // GPIO chip handle
    gpiod_line *pins[4] = {nullptr};        // GPIO lines for motor coils
    int gpio_pins[4];                       // GPIO pin numbers

    int step_delay = 1000;                  // Delay between steps (in µs)

    HighPrecisionTimer timer;               // Timer for step control

    // Step control state
    int currentStep = 0;                    // Current index in step sequence
    int stepCount = 0;                      // Number of steps taken
    int totalSteps = 0;                     // Target number of steps
    bool goingForward = true;               // Direction flag
    bool isBusy = false;                    // Whether motor is moving
    bool shouldStop = false;                // Whether motor should stop early

    mutable std::mutex cv_mutex;            // Mutex for condition variable
    std::condition_variable cv;             // Notifies waitUntilDone() when done

    std::atomic<bool> stopped{false};       // Ensures stop() logic runs once

    /**
     * @brief Outputs one step to GPIO based on the given pattern.
     * 
     * @param stepPattern An array of 4 values to write to the GPIO pins.
     */
    void step(int stepPattern[4]);

    /**
     * @brief Called by the timer at each interval.
     *        Executes one motor step and checks whether to stop.
     */
    void onStep();
};

#endif
