#ifndef THREAD_MPU_H
#define THREAD_MPU_H

#include "MPU6050.h"

class ThreadMPU : public MPU
{
private:
    IIC &iic;                             
    Kalman::KalmanFilter kfRoll, kfPitch; 
    MPU::AngleData calib;                 
    MPU::AngleData prevAngle;             
    float dt;                             
    std::vector<SensorCallback *> callback;
    std::thread workerThread;
    bool running = false;

public:

    ThreadMPU(IIC &iic_ref, float delta_time)
        : iic(iic_ref), dt(delta_time)
    {
        initKalmanFilter(kfRoll);
        initKalmanFilter(kfPitch);
    }

    void calibrate();
    void run();
    void start();
    void stop();
    void RegisterCallback(SensorCallback *cb);
};

#endif