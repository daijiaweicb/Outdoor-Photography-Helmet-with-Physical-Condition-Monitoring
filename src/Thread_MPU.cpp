#include "Thread_MPU.h"

void ThreadMPU::RegisterCallback(SensorCallback *cb)
{
    callback.push_back(cb);
}

void ThreadMPU::calibrate()
{
    calib = {0};
    MPU::calibrateSensors(iic, calib, 1000);
}

void ThreadMPU::run()
{
    running = true;
    while (running)
    {
        try
        {
            auto data = readMPU6050(iic);

            MPU::AngleData angle;
            {
                angle = calculateAngle(data, dt, prevAngle, calib, kfRoll, kfPitch);
                prevAngle = angle;
            }

            for (auto cb : callback)
            {
                cb->onSensorData(angle.pitch);
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
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