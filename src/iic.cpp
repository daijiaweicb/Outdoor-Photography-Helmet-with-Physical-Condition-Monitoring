#include "iic.h"

void IIC::iic_open()
{
    snprintf(filename, sizeof(filename), "/dev/i2c-%d", adapter_nr);
    file = open(filename, O_RDWR);
    if (file < 0)
    {
        std::cerr << "Can not open i2c device: " << filename << std::endl;
        exit(1);
    }

    //Set the address of MPU6050
    if (ioctl(file, I2C_SLAVE, 0x68) < 0)
    {
        std::cerr << "Failed to set I2C slave address" << std::endl;
        exit(1);
    }
    if (ioctl(file, I2C_SLAVE, 0x76) < 0)
    {
        std::cerr << "Failed to set I2C slave address" << std::endl;
        exit(1);
    }

    std::cout << "i2c device opened: " << filename << std::endl;
}

void IIC::iic_open_BMP()
{
    snprintf(filename, sizeof(filename), "/dev/i2c-%d", adapter_nr);
    file = open(filename, O_RDWR);
    if (file < 0)
    {
        std::cerr << "Can not open i2c device: " << filename << std::endl;
        exit(1);
    }

    // Set the address of BMP280
    if (ioctl(file, I2C_SLAVE, 0x76) < 0)
    {
        std::cerr << "Failed to set I2C slave address" << std::endl;
        exit(1);
    }

    std::cout << "i2c device opened: " << filename << std::endl;
}

void IIC::iic_close()
{
    if (file >= 0)
    {
        close(file);
        file = -1;
        std::cout << "i2c device closed" << std::endl;
    }
}

bool IIC::readRegisters(uint8_t reg, uint8_t *buffer, size_t length)
{
    if (write(file, &reg, 1) != 1)
    {
        std::cerr << "Error writing register address" << std::endl;
        return false;
    }
    if (read(file, buffer, length) != (ssize_t)length)
    {
        std::cerr << "Error reading registers" << std::endl;
        return false;
    }
    return true;
}

void IIC::iic_writeRegister(uint8_t reg, uint8_t value)
{
    uint8_t tmp[2] = {reg, value};
    if (write(file, tmp, 2) != 2)
    {
        throw std::runtime_error("Failed to write to i2c");
    }
}

uint8_t IIC::readByte(uint8_t reg)
{
    uint8_t value;
    if (write(file, &reg, 1) != 1)
        throw "register read successful";
    if (read(file, &value, 1) != 1)
        throw "fail to read register";
    return value;
}

uint16_t IIC::readU16(uint8_t reg)
{
    uint8_t buf[2];
    if (write(file, &reg, 1) != 1)
        throw "register read successful";
    if (read(file, buf, 2) != 2)
        throw "fail to read register";
    return (buf[1] << 8) | buf[0];
}

int16_t IIC::readS16(uint8_t reg)
{
    uint16_t val = readU16(reg);
    return (val > 32767) ? val - 65536 : val;
}