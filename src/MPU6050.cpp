#include "MPU6050.h"
#include <cmath>

void MPU::initMPU6050(IIC &iic)
{
    iic.iic_writeRegister(0x6B, 0x00); // Wake up
    iic.iic_writeRegister(0x37, 0x10);
    iic.iic_writeRegister(0x38, 0x01);
    iic.iic_writeRegister(0x1B, 0x00); //  ±250°/s
    iic.iic_writeRegister(0x1A, 0x03); // LowPass Filter 44Hz
    iic.iic_writeRegister(0x19, 0xC7); // Sampling Rate 5hz
    std::cout << "init success" << std::endl;
}

void MPU::beginMPU6050()
{
    // if(!iic_ptr) {
    //     iic_ptr = new IIC(1);
    //     owns_iic = true;
    //     iic_ptr->iic_open();
    // }

    
    iic.iic_open();
    initMPU6050(iic);

    chipGPIO = gpiod_chip_open_by_number(chipNo);
    if (!chipGPIO)
    {
        throw std::runtime_error("Failed to open GPIO chip");
    }

    pin = gpiod_chip_get_line(chipGPIO, Interupt_MPU);
    if (!pin)
    {
        gpiod_chip_close(chipGPIO);
        throw std::runtime_error("Failed to get GPIO line");
    }

    int ret = gpiod_line_request_rising_edge_events(pin, "Consumer");
    if (ret < 0)
    {
        std::cerr << "Could not request event" << std::endl;
    }

    calib = {0};
    calibrateSensors(iic, calib, 1000);

    std::cout << "Calibrate sensor success" << std::endl;

    prevAngle = {0, 0, 0};
    kal.initKalmanFilter(kfRoll);
    kal.initKalmanFilter(kfPitch);
    std::cout << "Kalman init success" << std::endl;

    str = std::thread(&MPU::worker, this);
    std::cout << "Thread init success" << std::endl;
}

void MPU::dataReady()
{

    static bool first_call = true;
    static auto prevTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float dt = 0.0f;

    if (!first_call)
    {
        dt = std::chrono::duration<float>(currentTime - prevTime).count();
    }
    else
    {
        first_call = false;
    }
    prevTime = currentTime;

    senda = readMPU6050(iic);

    angle = calculateAngle(senda, dt, prevAngle, calib, kfRoll, kfPitch);
    prevAngle = angle;

    if (callback != nullptr)
    {
        callback->SensorCallback(angle.roll);
    }
    else
    {
        std::cout <<"can not regist callback" << std::endl;
    }
}

// Read MPU6050 data: accelerometer and gyro totaling 14 bytes (6 bytes for accelerometer, 2 bytes for temperature, 6 bytes for gyro)
MPU::SensorData MPU::readMPU6050(IIC &iic)
{
    SensorData data;
    uint8_t buffer[14];
    if (!iic.readRegisters(0x3B, buffer, 14))
    {
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

// Calibrate gyroscope: calculate zero bias (requires sensor to be at rest)
void MPU::calibrateSensors(IIC &iic, AngleData &calib, int samples = 1000)
{
    float gx = 0, gy = 0, gz = 0;
    for (int i = 0; i < samples; i++)
    {
        uint8_t buffer[14];
        if (!iic.readRegisters(0x3B, buffer, 14))
        {
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
        usleep(10000); // Sampling interval 10ms
    }
    calib.gyroBiasX = gx / samples;
    calib.gyroBiasY = gy / samples;
    calib.gyroBiasZ = gz / samples;
}

// Calculate Roll and Pitch in degrees using accelerometer data.
float MPU::getAccRoll(float accelY, float accelZ)
{
    return atan2(accelY, accelZ) * 180.0f / M_PI;
}
float MPU::getAccPitch(float accelX, float accelY, float accelZ)
{
    return atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * 180.0f / M_PI;
}

// Calculate Roll, Pitch by fusing gyroscope integration with accelerometer measurements using Kalman filtering (Yaw simply integrates)
MPU::AngleData MPU::calculateAngle(const SensorData &data, float dt, const AngleData &prev,
                                   const AngleData &calib, Kalman::KalmanFilter &kfRoll, Kalman::KalmanFilter &kfPitch)
{
    MPU::AngleData angle;
    Kalman kalman;
    // Correct gyro data (remove calibration zero bias)
    float gyroX = data.gyroX - calib.gyroBiasX;
    float gyroY = data.gyroY - calib.gyroBiasY;
    float gyroZ = data.gyroZ - calib.gyroBiasZ;

    // Calculation of angles using accelerometers
    float accRoll = getAccRoll(data.accelY, data.accelZ);
    float accPitch = getAccPitch(data.accelX, data.accelY, data.accelZ);

    // Updating Roll and Pitch with Kalman Filtering
    angle.roll = kalman.kalmanUpdate(kfRoll, gyroX, dt, accRoll);
    angle.pitch = kalman.kalmanUpdate(kfPitch, gyroY, dt, accPitch);
    // Yaw using only simple integrals
    angle.yaw = prev.yaw + gyroZ * dt;

    return angle;
}

void MPU ::RegisterSetting(std::shared_ptr<CallbackInterface> cb)
{
    callback = cb;
    std::cout << "register Setting success" << std::endl;
}

