#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "MG90S_setting.h"
#include "MPU6050.h"
#include <QObject>

#pragma once

#include <QObject>
#include "MG90S_setting.h"
#include "MPU6050.h"

class MotorControl : public QObject, public MPU, public MPU::MPUCallbackInterface, public MG90S
{
    Q_OBJECT

private:
    struct Data {
        float PrevData;
        float NewData;
        float RevData;
        float target_angle; 
        float integral;
        float prev_error;
    };
    Data angle;

public:
    explicit MotorControl(QObject* parent = nullptr);

    void MPUCallback(AngleData &data) override;

signals:
    void temperatureUpdated(float temp);
};


#endif
