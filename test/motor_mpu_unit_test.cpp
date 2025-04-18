#include <gtest/gtest.h>
#include "motor_control.h"
#include "MPU6050.h"
#include "Mode.h"
#include "IServoControl.h"

// The mock servo class records the last set angle.
class MockServo : public IServoControl {
public:
    int last_angle = -1;

    void setAngle(int angle) override {
        last_angle = angle;
    }
};

// Testable version MotorControl
class TestableMotorControl : public MotorControl {
public:
    explicit TestableMotorControl(IServoControl *servo)
        : MotorControl(servo) {}

    using MotorControl::MPUCallback;
};

// ===================== test case =====================

// positive Volume Value
TEST(MotorControlTest, SetsCorrectAngleWithPositiveRoll)
{
    g_systemMode = SystemMode::Normal;
    MockServo servo;
    TestableMotorControl control(&servo);

    MPU::AngleData data{15.0f};
    data.temp = 28.0f;
    control.MPUCallback(data);

    EXPECT_EQ(servo.last_angle, 75); // 90 - 15
}

// negative Volume Value
TEST(MotorControlTest, SetsCorrectAngleWithNegativeRoll)
{
    g_systemMode = SystemMode::Normal;
    MockServo servo;
    TestableMotorControl control(&servo);

    MPU::AngleData data{-15.0f};
    data.temp = 28.0f;
    control.MPUCallback(data);

    EXPECT_EQ(servo.last_angle, 105); // 90 - (-15)
}

// Zero Volume Value
TEST(MotorControlTest, SetsCorrectAngleWithZeroRoll)
{
    g_systemMode = SystemMode::Normal;
    MockServo servo;
    TestableMotorControl control(&servo);

    MPU::AngleData data{0.0f};
    data.temp = 28.0f;
    control.MPUCallback(data);

    EXPECT_EQ(servo.last_angle, 90); // 90 - 0
}

// Fatigue mode constant angle
TEST(MotorControlTest, SetsCenterAngleInFatigueMode)
{
    g_systemMode = SystemMode::FatigueDetection;
    MockServo servo;
    TestableMotorControl control(&servo);

    MPU::AngleData data{180.0f};
    data.temp = 28.0f;
    control.MPUCallback(data);

    EXPECT_EQ(servo.last_angle, 90); 
}

// Call consecutively to see if the angle is correctly updated
TEST(MotorControlTest, HandlesSequentialMPUCallbacks)
{
    g_systemMode = SystemMode::Normal;
    MockServo servo;
    TestableMotorControl control(&servo);

    MPU::AngleData data1{10.0f};
    data1.temp = 28.0f;
    control.MPUCallback(data1);
    EXPECT_EQ(servo.last_angle, 80); // 90 - 10

    MPU::AngleData data2{20.0f};
    data2.temp = 28.0f;
    control.MPUCallback(data2);
    EXPECT_EQ(servo.last_angle, 70); // 90 - 20
}

