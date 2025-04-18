#ifndef _STEPMOTOR_SETTING_H
#define _STEPMOTOR_SETTING_H

#include <iostream>
#include <gpiod.h>
#include <unistd.h>

/**
 * @class StepperMotor
 * @brief Provides GPIO-based control for a 4-wire stepper motor using libgpiod.
 *
 * This class abstracts low-level GPIO operations for a unipolar/bipolar 4-phase
 * stepper motor. It supports initialization, stepping forward/backward, and
 * proper cleanup of GPIO lines. Rotation is achieved using an 8-step half-step sequence.
 */
class StepperMotor
{
private:
    gpiod_chip *chipGPIO = nullptr;  // GPIO chip handle
    gpiod_line *pins[4] = {nullptr}; // Handles to 4 GPIO lines
    int gpio_pins[4];                // GPIO pin numbers
    int step_delay = 1000;           // Delay per step in microseconds (default = 1ms)

public:
    /**
     * @brief Initializes the GPIO lines and prepares for motor control.
     *
     * @param chipNo GPIO chip index (e.g., 0 for /dev/gpiochip0)
     * @param pin1 GPIO line number for phase A
     * @param pin2 GPIO line number for phase B
     * @param pin3 GPIO line number for phase C
     * @param pin4 GPIO line number for phase D
     * @return true if initialization succeeds; false otherwise.
     */
    bool start(int chipNo, int pin1, int pin2, int pin3, int pin4);

    /**
     * @brief Rotates the motor forward by a specified number of steps.
     *
     * Uses a standard 8-step half-stepping sequence in clockwise direction.
     * @param steps Number of step cycles to execute.
     */
    void forward(int steps);

    /**
     * @brief Rotates the motor backward by a specified number of steps.
     *
     * Uses a reversed 8-step sequence in counterclockwise direction.
     * @param steps Number of step cycles to execute.
     */

    void backward(int steps);

    /**
     * @brief Releases all GPIO lines and stops the motor.
     *
     * Turns off all outputs and frees the requested GPIO resources.
     */
    void cleanup();
    private:
    /**
     * @brief Applies a given 4-phase step pattern to the GPIO lines.
     *
     * @param stepPattern An array of 4 binary values indicating output states for each phase.
     */
    void step(int stepPattern[4]);
};

#endif
