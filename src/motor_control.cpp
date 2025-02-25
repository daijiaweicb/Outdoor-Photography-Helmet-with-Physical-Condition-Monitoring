#include "motor_control.h"

MotorControl::MotorControl()
{

    angle.PrevData = 0;
}

void MotorControl::onSensorData(float value)
{
    // 更新角度数据
    angle.NewData = value;
    angle.RevData = angle.NewData - angle.PrevData;
    angle.PrevData = angle.NewData;

    std::cout << "Angle Change: " << angle.RevData << "°\n";

    // 检查是否达到操作间隔
    auto now = std::chrono::steady_clock::now();
    if (now - last_action_time < action_interval)
    {
        return; // 未到时间，直接返回
    }

    // // 执行舵机控制
    // float steps = angle.RevData / 5.625;
    // int intSteps = static_cast<int>(round(steps));

    // 更新时间戳
    last_action_time = now;
}

void MotorControl::setActionInterval(int milliseconds)
{
    action_interval = std::chrono::milliseconds(milliseconds);
}