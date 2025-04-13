#ifndef MOTOR_SENSOR_SERVICE_H
#define MOTOR_SENSOR_SERVICE_H

#include "motor_control.h"

class MotorSensorService
{
public:
    MotorSensorService();
    void start();
    void stop();

private:
    MG90S *servo;
    MotorControl motorapp;
};

#endif // MOTOR_SENSOR_SERVICE_H
