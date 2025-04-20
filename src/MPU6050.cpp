#include "MPU6050.h"
#include <cmath>
// MPU6050
void MPU::initMPU6050(IIC &iic)
{
    iic.iic_writeRegister(0x6B, 0x00); // Wake up
    iic.iic_writeRegister(0x37, 0x10); // Interrupt pin configuration (active high)
    iic.iic_writeRegister(0x38, 0x01); // Enable Data Ready Interrupt
    iic.iic_writeRegister(0x1B, 0x00); // ±250°/s
    iic.iic_writeRegister(0x1A, 0x03); // LowPass Filter 44Hz
    iic.iic_writeRegister(0x19, 0xF9); // Sampling Rate 4hz
}

    void MPU::beginMPU6050()
    {
        iic.iic_open(MPU_ADDRESS);
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
            gpiod_line_release(pin);
            gpiod_chip_close(chipGPIO);
            throw std::runtime_error("Could not request event");
        }

        calib = {0};
        void dataReady();
        int calib_init = calibrateSensors(iic, calib, 1000);
        if (!calib_init)
        {
            throw std::runtime_error("Calibrate sensor error");
        }

        prevAngle = {0, 0, 0};
        kal.initKalmanFilter(kfRoll);
        kal.initKalmanFilter(kfPitch);
    }

    void MPU::startWorker()
    {
        str = std::thread(&MPU::worker, this);
        std::cout << "thread start success" << std::endl;
        if (!str.joinable())
        {
            throw std::runtime_error("Failed to start worker thread");
        }
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
    angle.temp = senda.temp;
    prevAngle = angle;

    for (auto &cb : MPUcallback)
    {
        cb->MPUCallback(angle);
    }
}

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

    int16_t temperature = (buffer[6] << 8) | buffer[7];
    data.temp = temperature / 340.0 + 36.53;

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

//MPU6050 Initialization Calibration
bool MPU::calibrateSensors(IIC &iic, AngleData &calib, int samples)
{
    float gx = 0, gy = 0, gz = 0;
    using namespace std::chrono;
    auto next_time = steady_clock::now();

    for (int i = 0; i < samples; i++)
    {
        uint8_t buffer[14];
        if (!iic.readRegisters(0x3B, buffer, 14))
        {
            std::cerr << "Calibration read error" << std::endl;
            return false;
        }

        int16_t gx_raw = (buffer[8] << 8) | buffer[9];
        int16_t gy_raw = (buffer[10] << 8) | buffer[11];
        int16_t gz_raw = (buffer[12] << 8) | buffer[13];
        const float gyroScale = 250.0f / 32768.0f;

        gx += gx_raw * gyroScale;
        gy += gy_raw * gyroScale;
        gz += gz_raw * gyroScale;

        next_time += milliseconds(10);
        std::this_thread::sleep_until(next_time);//Only at Initialization Calibration, data reads are event triggered
    }

    calib.gyroBiasX = gx / samples;
    calib.gyroBiasY = gy / samples;
    calib.gyroBiasZ = gz / samples;
    return true;
}

float MPU::getAccRoll(float accelY, float accelZ)
{
    return atan2(accelY, accelZ) * 180.0f / M_PI;
}

float MPU::getAccPitch(float accelX, float accelY, float accelZ)
{
    return atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ)) * 180.0f / M_PI;
}

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

void MPU ::RegisterSetting(MPUCallbackInterface *ci)
{
    MPUcallback.push_back(ci);
}
