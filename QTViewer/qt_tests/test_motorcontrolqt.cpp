#include <gtest/gtest.h>
#include "Mode.h"
#include "MPU6050.h"

/**
 * @class MockMotorControlQT
 * @brief A lightweight mock of MotorControlQT for testing MPUCallback logic only.
 *
 * This class avoids dependencies on Qt or hardware, focusing purely on logic testing.
 */
class MockMotorControlQT {
public:
    int last_angle = -1;

    void MPUCallback(const MPU::AngleData& data) {
        if (g_systemMode == SystemMode::Normal) {
            last_angle = 90 + data.roll;
        } else if (g_systemMode == SystemMode::FatigueDetection) {
            last_angle = 90;
        }
    }
};

class MotorControlLogicTest : public ::testing::Test {
protected:
    MockMotorControlQT controller;
    MPU::AngleData data;

    void SetUp() override {
        data = {0.0f, 36.5f};
    }
};

TEST_F(MotorControlLogicTest, NormalModeAngleComputation) {
    g_systemMode = SystemMode::Normal;
    data.roll = 30.0f;

    controller.MPUCallback(data);

    EXPECT_EQ(controller.last_angle, 120) << "Expected 90 + 30 roll in Normal mode";
}

TEST_F(MotorControlLogicTest, FatigueModeLocksAt90Degrees) {
    g_systemMode = SystemMode::FatigueDetection;
    data.roll = 45.0f;

    controller.MPUCallback(data);

    EXPECT_EQ(controller.last_angle, 90) << "Expected fixed 90° in FatigueDetection mode";
}
