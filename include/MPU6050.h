#ifndef MPU6050_H
#define MPU6050_H

#include <gpiod.h>
#include <thread>
#include <memory>
#include <vector>
#include "iic.h"
#include "MPU_kalman.h"

/**
 * Use Pin 6 of raspberry pi as interrupt pin
 */
#define MPU_ADDRESS 0x68
#define Interupt_MPU 6
#define chipNo 0

class MotorControl;
class MPUCallbackInterface;

/**
 * @class MPU
 * @brief MPU6050 6-axis motion sensor control class
 *
 * This class provides complete control of the MPU6050 sensor, including hardware initialization, data acquisition,
 * Sensor calibration, data fusion (Kalman filtering) and asynchronous data reporting functions.
 *
 * @note Typical usage flow:
 * 1. Construct the MPU object
 * 2. Call beginMPU6050() to initialize the hardware.
 * 3. Call RegisterSetting() to register the data callback interface.
 * 4. Automatically report the processed sensor data through the worker thread.
 *
 */
class MPU
{
public:
    struct SensorData
    {
        float gyroX, gyroY, gyroZ;
        float accelX, accelY, accelZ;
        float temp;
    };

    struct AngleData
    {
        float roll, pitch, yaw;
        float gyroBiasX, gyroBiasY, gyroBiasZ;
        float temp;
    };

    /**
     *
     * @brief MPU6050 control class
     *
     * @note workflow:
     * 1. beginMPU6050() Initialize hardware
     * 2. RegisterSetting() Registration Data Callback
     * 3. Automatic reporting of sensor data via worker threads
     */
    void beginMPU6050();

    /**
     * @brief Register callback for mpu6050
     * @param  {CallbackInterface*} ci :
     */
    void RegisterSetting(MPUCallbackInterface *ci);

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

    std::vector<MPUCallbackInterface *> MPUcallbackinterface;
    gpiod_chip *chipGPIO = nullptr;
    gpiod_line *pin = nullptr;


    bool running = false;

    // Sensor Data
    AngleData calib;
    AngleData angle;
    AngleData prevAngle;
    SensorData senda;

    // Filter assembly
    Kalman kal;
    Kalman::KalmanFilter kfRoll;
    Kalman::KalmanFilter kfPitch;

    // callback interface

    std::thread str;

    /**
     * @brief MPU6050 by writing registers
     * @param  {IIC} iic :
     */
    void initMPU6050(IIC &iic);

    /**
     * @brief Every time this function is triggered, it means that the data of mpu6050 is read once.
     *
     * @note workflow:
     * 1. calculte the angle
     * 2. send data to callback function
     */
    void dataReady();

    /**
     * @brief Read data from MPU6050 by reading register 0x3B
     *
     *
     * @param  {IIC} iic     :
     * @return {SensorData}  :
     * @note data structure:
     * 1. Can read accelerometer data and gyro data totaling 14 bytes (6 bytes for accelerometer, 2 bytes for temperature, 6 bytes for gyro)
     * 2. Can also read temperature data if you want
     */
    SensorData readMPU6050(IIC &iic);

    /**
     * @brief Calibrate gyroscope: calculate zero bias (requires sensor to be at rest).
     * success calibrate return true, fail return false
     * @param  {IIC} iic         :
     * @param  {AngleData} calib :
     * @param  {int} samples     :
     * @return {bool}            :
     */
    bool calibrateSensors(IIC &iic, AngleData &calib, int samples);

    /**
     * @brief Calculate Roll  in degrees using accelerometer data.
     * @param  {float} accelY :
     * @param  {float} accelZ :
     * @return {float}        :
     */
    float getAccRoll(float accelY, float accelZ);

    /**
     * @brief Calculate Pitch in degrees using accelerometer data.
     * @param  {float} accelX :
     * @param  {float} accelY :
     * @param  {float} accelZ :
     * @return {float}        :
     */
    float getAccPitch(float accelX, float accelY, float accelZ);

    /**
     * @brief Calculate Roll, Pitch by fusing gyroscope integration with accelerometer measurements using Kalman filtering (Yaw simply integrates)
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

    /**
     * @brief Data Processing Work Thread Entry
     *
     * Workflow:
     * 1. wait for GPIO interrupt event
     * 2. Trigger dataReady() when a rising edge is detected.
     *
     * @note Control thread lifecycle by running flag.
     */
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

class MPUCallbackInterface
{
public:
    virtual void MPUCallback(float &data) = 0;
    virtual ~MPUCallbackInterface() = default;
};

#endif
