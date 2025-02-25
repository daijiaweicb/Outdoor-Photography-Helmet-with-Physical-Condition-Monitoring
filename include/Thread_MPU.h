#ifndef THREAD_MPU_H
#define THREAD_MPU_H

#include "MPU6050.h"

class ThreadMPU : public MPU
{
private:
    mutable std::mutex callback_mutex;
    mutable std::mutex data_mutex;
    IIC &iic;                             // 引用外部IIC对象
    Kalman::KalmanFilter kfRoll, kfPitch; // Kalman滤波器作为成员
    MPU::AngleData calib;                 // 校准数据
    MPU::AngleData prevAngle;             // 上一时刻角度
    float dt;                             // 采样周期
    std::vector<SensorCallback *> callback;
    std::thread workerThread;
    bool running = false;

public:
    // 通过构造函数初始化必要资源
    ThreadMPU(IIC &iic_ref, float delta_time)
        : iic(iic_ref), dt(delta_time)
    {
        initKalmanFilter(kfRoll); // 使用继承自Kalman的方法
        initKalmanFilter(kfPitch);
    }

    void calibrate();
    void run();
    void start();
    void stop();
    void RegisterCallback(SensorCallback *cb);
};

#endif