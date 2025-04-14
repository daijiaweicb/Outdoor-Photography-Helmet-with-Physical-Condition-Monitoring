#ifndef MOTOR_SENSOR_SERVICE_H
#define MOTOR_SENSOR_SERVICE_H

#include "motor_control_QT.h"

class MotorSensorService
{
public:
    MotorSensorService();
    void start();
    void stop();
    MotorControlQT* getMotorControl();

private:
    MG90S *servo;
    MotorControlQT motorapp;

};

#endif // MOTOR_SENSOR_SERVICE_H
