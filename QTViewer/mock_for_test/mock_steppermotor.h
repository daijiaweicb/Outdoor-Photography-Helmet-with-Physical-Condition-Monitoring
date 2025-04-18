#pragma once
#include "Stepmotor_setting.h"

/**
 * @class MockStepperMotor
 * @brief A mock implementation of the StepperMotor class used for unit testing.
 *
 * This mock class records whether its methods were called, enabling verification
 * of motor control logic without interacting with real hardware.
 */
class MockStepperMotor : public StepperMotor
{
public:
    bool start_called = false;
    bool forward_called = false;
    bool backward_called = false;
    bool cleanup_called = false;

    bool start(int en, int in1, int in2, int in3, int in4)
    {
        start_called = true;
        return true;
    }

    void forward(int steps)
    {
        forward_called = true;
    }

    void backward(int steps)
    {
        backward_called = true;
    }

    void cleanup()
    {
        cleanup_called = true;
    }
};
