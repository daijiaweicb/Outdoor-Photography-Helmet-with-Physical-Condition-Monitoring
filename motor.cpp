#include "motor.h"
#include <cmath>

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

void StepperMotor::step(int stepPattern[4])
{
    for (int i = 0; i < 4; i++)
    {
        gpiod_line_set_value(pins[i], stepPattern[i]);
    }
    usleep(step_delay);
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


    for (int i = 0; i < steps; i++)
    {
        step(stepSequence[i % 8]);
    }
}

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

void StepperMotor::cleanup()
{
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

void MotorControl::onSensorData(float value)
{
    Data angle;
    StepperMotor motor;
    angle.NewData = value;
    angle.RevData = angle.NewData - angle.PrevData;
    angle.PrevData = angle.NewData;

    cout << "Now The Angles is: " << angle.NewData<<endl;
    float steps = angle.RevData /5.625;
    int intSteps = static_cast<int>(round(steps));

    if(intSteps > 0)
    {
        motor.forward(intSteps);
    }
    else if(intSteps < 0)
    {
        motor.backward(-intSteps);
    }

}
