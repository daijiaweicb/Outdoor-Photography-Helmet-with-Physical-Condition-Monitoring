#include "motor.h"
#include <cmath>
#include <chrono>

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

MotorControl::MotorControl()
{
    
    angle.PrevData =0;
}

// motor.h
#include <chrono>

class MotorControl : public SensorCallback {
private:
    Data angle;
    std::chrono::steady_clock::time_point last_action_time;  // 记录上次操作时间
    const std::chrono::milliseconds action_interval{100};    // 操作间隔 100ms
public:
    void onSensorData(float value) override;
};

// motor.cpp
void MotorControl::onSensorData(float value) {
    // 更新角度数据
    angle.NewData = value;
    angle.RevData = angle.NewData - angle.PrevData;
    angle.PrevData = angle.NewData;
    
    std::cout << "Angle Change: " << angle.RevData << "°\n";

    // 检查是否达到操作间隔
    auto now = std::chrono::steady_clock::now();
    if (now - last_action_time < action_interval) {
        return;  // 未到时间，直接返回
    }

    // // 执行舵机控制
    // float steps = angle.RevData / 5.625;
    // int intSteps = static_cast<int>(round(steps));


    // 更新时间戳
    last_action_time = now;
}
