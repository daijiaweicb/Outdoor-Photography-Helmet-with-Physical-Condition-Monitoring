#include "Stepmotor_setting.h"

//Function: Implement 4-phase stepper motor control
using namespace std;
// Initialize GPIO and setup motor control pins
bool StepperMotor::start(int chipNo, int pin1, int pin2, int pin3, int pin4)
{
    // Open GPIO chip by number
    chipGPIO = gpiod_chip_open_by_number(chipNo);
    if (!chipGPIO)
    {
        cout << "Failed to open GPIO chip" << endl;
        return false;
    }
    // Store four control pin numbers
    gpio_pins[0] = pin1;
    gpio_pins[1] = pin2;
    gpio_pins[2] = pin3;
    gpio_pins[3] = pin4;

    // Configure four GPIO pins in loop
    for (int i = 0; i < 4; i++)
    {
        // Get GPIO line object
        pins[i] = gpiod_chip_get_line(chipGPIO, gpio_pins[i]);
        if (!pins[i])
        {
            cout << "Failed to get GPIO line " << gpio_pins[i] << endl;
            cleanup();
            return false;
        }
        // Set pin as output mode
        if (gpiod_line_request_output(pins[i], "stepper_motor", 0) < 0)
        {
            cout << "Failed to set GPIO " << gpio_pins[i] << " as output" << endl;
            cleanup();
            return false;
        }
    }

    return true;
}
// Perform single step operation
void StepperMotor::step(int stepPattern[4])
{
    // Set output levels for four pins
    for (int i = 0; i < 4; i++)
    {
        gpiod_line_set_value(pins[i], stepPattern[i]);
    }
    usleep(step_delay);    //Maintain step interval
}

void StepperMotor::forward(int steps)
{
    int stepSequence[8][4] = {
    {1, 0, 0, 0},  // Step 1
    {1, 1, 0, 0},  // Step 2
    {0, 1, 0, 0},  // Step 3
    {0, 1, 1, 0},  // Step 4
    {0, 0, 1, 0},  // Step 5
    {0, 0, 1, 1},  // Step 6
    {0, 0, 0, 1},  // Step 7
    {1, 0, 0, 1}   // Step 8
};

    // Execute step sequence in loop
    for (int i = 0; i < steps; i++)
    {
        step(stepSequence[i % 8]);    //Use modulo 8 for cycling
    }
}
// Rotate backward for specified steps
void StepperMotor::backward(int steps)
{
    int stepSequenceReverse[8][4] = {
    {1, 0, 0, 1},  // Step 1
    {0, 0, 0, 1},  // Step 2
    {0, 0, 1, 1},  // Step 3
    {0, 0, 1, 0},  // Step 4
    {0, 1, 1, 0},  // Step 5
    {0, 1, 0, 0},  // Step 6
    {1, 1, 0, 0},  // Step 7
    {1, 0, 0, 0}   // Step 8
};


    for (int i = 0; i < steps; i++)
    {
        step(stepSequenceReverse[i % 8]);
    }
}
// Cleanup GPIO resources
void StepperMotor::cleanup()
{
    // Reset all pins and release resources
    for (int i = 0; i < 4; i++)
    {
        if (pins[i])
        {
            gpiod_line_set_value(pins[i], 0);    //Set low level
            gpiod_line_release(pins[i]);         //Release pin
        }
    }
    if (chipGPIO)
    {
        gpiod_chip_close(chipGPIO);    //Close GPIO chip
    }
}
