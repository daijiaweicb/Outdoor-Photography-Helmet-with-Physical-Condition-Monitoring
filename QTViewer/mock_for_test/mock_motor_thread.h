#pragma once
#include "motor_thread.h"
#include "mock_steppermotor.h"

/**
 * @class TestableMotorThread
 * @brief A test-friendly subclass of MotorThread that allows replacing the internal StepperMotor with a mock.
 *
 * This class is used in unit tests to simulate motor behavior without accessing real GPIO hardware.
 */
class TestableMotorThread : public MotorThread {
public:
    MockStepperMotor mock_motor;

    TestableMotorThread(QObject* parent = nullptr)
        : MotorThread(parent)
    {
        this->setMotor(&mock_motor);
    }

    void setMotor(StepperMotor* m) {
        this->motor = *m;
    }

    MockStepperMotor* getMockMotor() {
        return &mock_motor;
    }
};
