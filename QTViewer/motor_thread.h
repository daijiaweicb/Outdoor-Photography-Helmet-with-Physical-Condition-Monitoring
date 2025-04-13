#ifndef MOTOR_THREAD_H
#define MOTOR_THREAD_H

#include <QThread>
#include "Stepmotor_setting.h"
#include "MG90S_setting.h"
#include "Mode.h"

class MotorThread : public QThread
{
    Q_OBJECT
public:
    MotorThread(QObject *parent = nullptr,MG90S* sharedServo);
    ~MotorThread();

    void run() override;

signals:
    void modeChanged(SystemMode newMode);

private:
    StepperMotor motor;
    MG90S *servo = nullptr;
};

#endif // MOTOR_THREAD_H
