#include "Thread_MPU.h"

void ThreadMPU::RegisterCallback(SensorCallback *cb)
{
    std::lock_guard<std::mutex> lock(callback_mutex);
    callback.push_back(cb);
}

void ThreadMPU::calibrate()
{
    calib = {0};
    MPU::calibrateSensors(iic, calib, 1000); // 调用基类方法
}

void ThreadMPU::run()
{
    running = true;
    while (running)
    {
        auto cycle_start = std::chrono::steady_clock::now();

        try
        {
            // 读取传感器数据
            auto data = readMPU6050(iic);

            // 计算角度
            MPU::AngleData angle;
            {
                std::lock_guard<std::mutex> lock(data_mutex);
                angle = calculateAngle(data, dt, prevAngle, calib, kfRoll, kfPitch);
                prevAngle = angle;
            }

            // 安全获取回调列表
            std::vector<SensorCallback *> local_callbacks;
            {
                std::lock_guard<std::mutex> lock(callback_mutex);
                local_callbacks = callback;
            }

            // 触发回调
            for (auto cb : local_callbacks)
            {
                cb->onSensorData(angle.pitch);
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        // // 精确周期控制
        // auto elapsed = std::chrono::steady_clock::now() - cycle_start;
        // std::this_thread::sleep_for(std::chrono::milliseconds(10) - elapsed);
    }
}

void ThreadMPU ::start()
{
    workerThread = std::thread(&ThreadMPU::run, this);
}

void ThreadMPU ::stop()
{
    running = false;
    if (workerThread.joinable())
    {
        workerThread.join();
    }
}