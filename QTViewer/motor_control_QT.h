#ifndef MOTOR_CONTROL_QT_H
#define MOTOR_CONTROL_QT_H

#include "MG90S_setting.h"
#include "MPU6050.h"
#include <QObject>

//For QT inferface

class MotorControlQT : public QObject, public MPU, public MPU::MPUCallbackInterface, public MG90S
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
    explicit MotorControlQT(QObject* parent = nullptr);

    void MPUCallback(AngleData &data) override;

Q_SIGNALS:
    void temperatureUpdated(float temp);
};


#endif
