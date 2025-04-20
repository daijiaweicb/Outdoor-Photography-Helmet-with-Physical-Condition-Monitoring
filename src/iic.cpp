#include "iic.h"
// I2C communication interface implementation for Linux systems
// Open I2C bus and set slave address
void IIC::iic_open(uint8_t sensorAddress)
{
    // Generate device file path
    snprintf(filename, sizeof(filename), "/dev/i2c-%d", adapter_nr);
    // Open I2C device file with read/write access
    file = open(filename, O_RDWR);
    if (file < 0)
    {
        std::cerr << "Can not open i2c device: " << filename << std::endl;
        exit(1);
    }

    //Set the address of sensor
    if (ioctl(file, I2C_SLAVE, sensorAddress) < 0)
    {
        std::cerr << "Failed to set I2C slave address" << std::endl;
        exit(1);
    }

    std::cout << "i2c device opened: " << filename << std::endl;
}

void IIC::iic_open_BMP()
{
    // Same path generation as generic open
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
// Close I2C device
void IIC::iic_close()
{
    if (file >= 0)
    {
        close(file);
        file = -1;
        std::cout << "i2c device closed" << std::endl;
    }
}
// Read multiple registers from I2C device
bool IIC::readRegisters(uint8_t reg, uint8_t *buffer, size_t length)
{
    // Write target register address
    if (write(file, &reg, 1) != 1)
    {
        std::cerr << "Error writing register address" << std::endl;
        return false;
    }
    // Read specified number of bytes from register
    if (read(file, buffer, length) != (ssize_t)length)
    {
        std::cerr << "Error reading registers" << std::endl;
        return false;
    }
    return true;
}
// Write single register value
void IIC::iic_writeRegister(uint8_t reg, uint8_t value)
{
    uint8_t tmp[2] = {reg, value}; // Combine address and data
    if (write(file, tmp, 2) != 2)
    {
        throw std::runtime_error("Failed to write to i2c");
    }
}
// Read single byte from register
uint8_t IIC::readByte(uint8_t reg)
{
    uint8_t value;
    // Select register to read
    if (write(file, &reg, 1) != 1)
        throw "register read successful";
    if (read(file, &value, 1) != 1)
        throw "fail to read register";
    return value;
}
// Read unsigned 16-bit value
uint16_t IIC::readU16(uint8_t reg)
{
    uint8_t buf[2];
    if (write(file, &reg, 1) != 1)
        throw "register read successful";
    if (read(file, buf, 2) != 2)
        throw "fail to read register";
    return (buf[1] << 8) | buf[0];
}
// Read signed 16-bit value
int16_t IIC::readS16(uint8_t reg)
{
    uint16_t val = readU16(reg);
    return (val > 32767) ? val - 65536 : val;
}
