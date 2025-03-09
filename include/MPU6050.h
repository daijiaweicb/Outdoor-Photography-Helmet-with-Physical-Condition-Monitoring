#ifndef MPU6050_H
#define MPU6050_H

#include <gpiod.h>
#include <thread>
#include <memory>
#include "iic.h"
#include "MPU_kalman.h"

/**
 * Use Pin 6 as interrupt pin 
 */


#define Interupt_MPU 6
#define chipNo 0

class CallbackInterface
{
public:
    virtual void SensorCallback(float value) = 0;
    virtual ~CallbackInterface() = default;
};

class MPU
{
public:
    IIC iic;
    struct SensorData
    {
        float gyroX, gyroY, gyroZ;
        float accelX, accelY, accelZ;
    };

    struct AngleData
    {
        float roll, pitch, yaw;
        float gyroBiasX, gyroBiasY, gyroBiasZ;
    };

    std::thread str;
    void initMPU6050(IIC &iic);
    void beginMPU6050();
    void dataReady();
    SensorData readMPU6050(IIC &iic);
    void calibrateSensors(IIC &iic, AngleData &calib, int samples);
    float getAccRoll(float accelY, float accelZ);
    float getAccPitch(float accelX, float accelY, float accelZ);
    AngleData calculateAngle(const SensorData &data, float dt, const AngleData &prev,
                             const AngleData &calib, Kalman::KalmanFilter &kfRoll, Kalman::KalmanFilter &kfPitch);
    void worker()
    {
        running = true;
        while (running)
        {
            const struct timespec ts = {1, 0};
            int r = gpiod_line_event_wait(pin, &ts);
            if (1 == r)
            {
                struct gpiod_line_event event;
                gpiod_line_event_read(pin, &event);
                dataReady();
            }
            else
            {
                running = false;
            }
        }
    }

    void RegisterSetting(std::shared_ptr<CallbackInterface> cb);

    MPU() : iic(1)
    {
    }

    ~MPU()
    {
        running = false;
        if (str.joinable())
        {
            str.join();
        }
        if (pin)
        {
            gpiod_line_release(pin);
            pin = nullptr;
        }
        if (chipGPIO)
        {
            gpiod_chip_close(chipGPIO);
            chipGPIO = nullptr;
        }
        if (owns_iic && iic_ptr)
        {
            iic_ptr->iic_close();
            delete iic_ptr;
        }
    }

private:
    IIC *iic_ptr = nullptr;
    bool owns_iic = false;

    gpiod_chip *chipGPIO = nullptr;
    gpiod_line *pin = nullptr;
    bool running = false;

    AngleData calib;
    AngleData angle;
    AngleData prevAngle;
    SensorData senda;

    Kalman kal;
    Kalman::KalmanFilter kfRoll;
    Kalman::KalmanFilter kfPitch;

    std::shared_ptr<CallbackInterface> callback; 
};

#endif
