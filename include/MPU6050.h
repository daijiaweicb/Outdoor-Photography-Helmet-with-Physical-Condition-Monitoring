#ifndef MPU6050_H
#define MPU6050_H

#include <gpiod.h>
#include <thread>
#include <memory>
#include "iic.h"
#include "MPU_kalman.h"

/**
 * Use Pin 6 of raspberry pi as interrupt pin
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

    /**
     * Use this function to start MPU6050
     * 1. init iic
     * 2. init MPU6050
     * 3. open chip and gpio
     * 4. request rising edge event
     * 5. init kalman filter
     * 6. start worker thread
     */
    void beginMPU6050();

    /**
     * Registering callback for MPU6050
     * @param  {std::shared_ptr<CallbackInterface>} cb : 
     */
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
    }

private:
    IIC iic;

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

    std::thread str;
    
    /**
     * init MPU6050 by writing registers
     * @param  {IIC} iic : 
     */
    void initMPU6050(IIC &iic);

    /**
     * Every time this function is triggered, it means that the data of mpu6050 is read once.
     * 1. calculte the angle
     * 2. send data to callback function
     */
    void dataReady();

    /**
     * Read data from MPU6050 by reading register 0x3B
     * Can read accelerometer data and gyro data totaling 14 bytes (6 bytes for accelerometer, 2 bytes for temperature, 6 bytes for gyro)
     * Can also read temperature data if you want
     * @param  {IIC} iic     : 
     * @return {SensorData}  : 
     */
    SensorData readMPU6050(IIC &iic);

    /**
     *  Calibrate gyroscope: calculate zero bias (requires sensor to be at rest)
     *  success calibrate return true, fail return false
     * @param  {IIC} iic         : 
     * @param  {AngleData} calib : 
     * @param  {int} samples     : 
     * @return {bool}            : 
     */
    bool calibrateSensors(IIC &iic, AngleData &calib, int samples);

    /**
     * // Calculate Roll  in degrees using accelerometer data.
     * @param  {float} accelY : 
     * @param  {float} accelZ : 
     * @return {float}        : 
     */
    float getAccRoll(float accelY, float accelZ);

    /**
     * Calculate Pitch in degrees using accelerometer data.
     * @param  {float} accelX : 
     * @param  {float} accelY : 
     * @param  {float} accelZ : 
     * @return {float}        : 
     */
    float getAccPitch(float accelX, float accelY, float accelZ);
    
    /**
     * Calculate Roll, Pitch by fusing gyroscope integration with accelerometer measurements using Kalman filtering (Yaw simply integrates)
     * @param  {SensorData} data              : 
     * @param  {float} dt                     : 
     * @param  {AngleData} prev               : 
     * @param  {AngleData} calib              : 
     * @param  {Kalman::KalmanFilter} kfRoll  : 
     * @param  {Kalman::KalmanFilter} kfPitch : 
     * @return {AngleData}                    : 
     */
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
};

#endif
