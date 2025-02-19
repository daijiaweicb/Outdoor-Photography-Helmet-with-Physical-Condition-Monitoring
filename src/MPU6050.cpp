#include "MPU6050.h"
#include <cmath>
#include "Test.h"

void Kalman::initKalmanFilter(KalmanFilter &kf)
{
    kf.angle = 0.0f;
    kf.bias = 0.0f;
    kf.P[0][0] = 0.0f; kf.P[0][1] = 0.0f;
    kf.P[1][0] = 0.0f; kf.P[1][1] = 0.0f;
    kf.Q_angle = 0.001f;
    kf.Q_bias = 0.003f;
    kf.R_measure = 0.03f;
}

float Kalman::kalmanUpdate(KalmanFilter &kf, float newRate, float dt, float measuredAngle)
{
    kf.angle += dt * (newRate - kf.bias);
    
    // Updating the error covariance matrix
    kf.P[0][0] += dt * (dt * kf.P[1][1] - kf.P[0][1] - kf.P[1][0] + kf.Q_angle);
    kf.P[0][1] -= dt * kf.P[1][1];
    kf.P[1][0] -= dt * kf.P[1][1];
    kf.P[1][1] += kf.Q_bias * dt;
    
    // Update
    float S = kf.P[0][0] + kf.R_measure;
    float K0 = kf.P[0][0] / S;
    float K1 = kf.P[1][0] / S;
    
    float y = measuredAngle - kf.angle;
    kf.angle += K0 * y;
    kf.bias  += K1 * y;
    
    float P00_temp = kf.P[0][0];
    float P01_temp = kf.P[0][1];
    
    kf.P[0][0] -= K0 * P00_temp;
    kf.P[0][1] -= K0 * P01_temp;
    kf.P[1][0] -= K1 * P00_temp;
    kf.P[1][1] -= K1 * P01_temp;
    
    return kf.angle;
}


// Calibrate gyroscope: calculate zero bias (requires sensor to be at rest)
void MPU::calibrateSensors(IIC &iic, AngleData &calib, int samples = 1000)
{
    float gx = 0, gy = 0, gz = 0;
    for (int i = 0; i < samples; i++) {
        uint8_t buffer[14];
        if(!iic.readRegisters(0x3B, buffer, 14)) {
            std::cerr << "Calibration read error" << std::endl;
            continue;
        }
        // MPU6050 register 0x43 starts with gyro data (skips temperature register)
        int16_t gx_raw = (buffer[8] << 8) | buffer[9];
        int16_t gy_raw = (buffer[10] << 8) | buffer[11];
        int16_t gz_raw = (buffer[12] << 8) | buffer[13];
        const float gyroScale = 250.0f / 32768.0f; // ±250°/s
        gx += gx_raw * gyroScale;
        gy += gy_raw * gyroScale;
        gz += gz_raw * gyroScale;
        usleep(10000);  // Sampling interval 10ms
    }
    calib.gyroBiasX = gx / samples;
    calib.gyroBiasY = gy / samples;
    calib.gyroBiasZ = gz / samples;
}


void MPU::initMPU6050(IIC &iic)
{
    iic.iic_writeRegister(0x6B, 0x00);  // Wake up
    iic.iic_writeRegister(0x1B, 0x00);  //  ±250°/s
    iic.iic_writeRegister(0x1A, 0x03);  // LowPass Filter 44Hz
    iic.iic_writeRegister(0x19, 0x00);  // Sampling Rate 1kHz
}

// Read MPU6050 data: accelerometer and gyro totaling 14 bytes (6 bytes for accelerometer, 2 bytes for temperature, 6 bytes for gyro)
MPU::SensorData MPU::readMPU6050(IIC &iic)
{
    SensorData data;
    uint8_t buffer[14];
    if(!iic.readRegisters(0x3B, buffer, 14)) {
        throw std::runtime_error("Failed to read sensor data");
    }
    // Accelerometer data (assumed range ±2g, conversion factor 1/16384)
    int16_t ax_raw = (buffer[0] << 8) | buffer[1];
    int16_t ay_raw = (buffer[2] << 8) | buffer[3];
    int16_t az_raw = (buffer[4] << 8) | buffer[5];
    const float accelScale = 1.0f / 16384.0f;
    data.accelX = ax_raw * accelScale;
    data.accelY = ay_raw * accelScale;
    data.accelZ = az_raw * accelScale;
    
    // Gyro data (starting at register 0x43)
    int16_t gx_raw = (buffer[8] << 8) | buffer[9];
    int16_t gy_raw = (buffer[10] << 8) | buffer[11];
    int16_t gz_raw = (buffer[12] << 8) | buffer[13];
    const float gyroScale = 250.0f / 32768.0f;
    data.gyroX = gx_raw * gyroScale;
    data.gyroY = gy_raw * gyroScale;
    data.gyroZ = gz_raw * gyroScale;
    
    return data;
}

// Calculate Roll and Pitch in degrees using accelerometer data.  
float MPU::getAccRoll(float accelY, float accelZ) 
{
    return atan2(accelY, accelZ) * 180.0f / M_PI;
}
float MPU:: getAccPitch(float accelX, float accelY, float accelZ)
{
    return atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * 180.0f / M_PI;
}


// Calculate Roll, Pitch by fusing gyroscope integration with accelerometer measurements using Kalman filtering (Yaw simply integrates)
MPU::AngleData Kalman::calculateAngle(const SensorData &data, float dt, const AngleData &prev, 
                           const AngleData &calib, KalmanFilter &kfRoll, KalmanFilter &kfPitch)
{
    AngleData angle;
    // Correct gyro data (remove calibration zero bias)
    float gyroX = data.gyroX - calib.gyroBiasX;
    float gyroY = data.gyroY - calib.gyroBiasY;
    float gyroZ = data.gyroZ - calib.gyroBiasZ;
    
    // Calculation of angles using accelerometers
    float accRoll  = mpu. getAccRoll(data.accelY, data.accelZ);
    float accPitch = mpu. getAccPitch(data.accelX, data.accelY, data.accelZ);
    
    // Updating Roll and Pitch with Kalman Filtering
    angle.roll  = kalmanUpdate(kfRoll, gyroX, dt, accRoll);
    angle.pitch = kalmanUpdate(kfPitch, gyroY, dt, accPitch);
    // Yaw using only simple integrals
    angle.yaw = prev.yaw + gyroZ * dt;
    
    return angle;
}
