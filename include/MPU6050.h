#ifndef MPU6050_H
#define MPU6050_H

#include "iic.h"
#include <iostream>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>

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

    virtual ~MPU() = default; 

    void calibrateSensors(IIC &iic, AngleData &calib, int samples);
    void initMPU6050(IIC &iic);
    SensorData readMPU6050(IIC &iic);
    float getAccRoll(float accelY, float accelZ);
    float getAccPitch(float accelX, float accelY, float accelZ);
};

class Kalman : public MPU
{
public:
    MPU mpu;
    struct KalmanFilter
    {
        float angle;      // Estimated perspective
        float bias;       // Estimated gyro zero bias
        float P[2][2];    // Error covariance matrix
        float Q_angle;    // Process noise variance (angle)
        float Q_bias;     // Process noise variance (zero bias)
        float R_measure;  // Observation noise variance
    };

    void initKalmanFilter(KalmanFilter &kf);
    float kalmanUpdate(KalmanFilter &kf, float newRate, float dt, float measuredAngle);
    AngleData calculateAngle(const SensorData &data, float dt, const AngleData &prev,
                             const AngleData &calib, KalmanFilter &kfRoll, KalmanFilter &kfPitch);
};

#endif // MPU6050_H
