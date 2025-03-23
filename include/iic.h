#ifndef IIC_H
#define IIC_H

/**
 * The I2C is controlled by a kernel driver
 * Each I2C adapter gets a number, for Raspberry pi, the number of the adapter is 1
 */

#include <iostream>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <sys/ioctl.h>
#include <cstring>
#include <cstdint>
#include <unistd.h>

class IIC
{

private:
    int adapter_nr;
    char filename[20];

public:
    int file;
    IIC(int adapter) : adapter_nr(adapter), file(-1) {}

    /**
     * @brief open the I2C device
     * @param  {uint8_t} sensorAddress : 
     * @note input value is the address of the sensor
     */
    void iic_open(uint8_t sensorAddress);

    void iic_open_BMP();

    /**
     * @brief Close the I2C device
     */
    void iic_close();

    /**
     * @brief read the register
     * @param  {uint8_t} reg     :
     * @param  {uint8_t*} buffer :
     * @param  {size_t} length   :
     * @return {bool}            :
     */
    bool readRegisters(uint8_t reg, uint8_t *buffer, size_t length);
    
    /**
     * @brief wite the register
     * @param  {uint8_t} reg   :
     * @param  {uint8_t} value :
     */
    void iic_writeRegister(uint8_t reg, uint8_t value);

    /**
     * @brief read byte form the register
     * @param  {uint8_t} reg : 
     * @return {uint8_t}     : 
     */
    uint8_t readByte(uint8_t reg);

    /**
     * @brief Read 16-bit unsigned integer
     * @param  {uint8_t} reg : 
     * @return {uint16_t}    : 
     */
    uint16_t readU16(uint8_t reg);

    /**
     * @brief Read 16-bit signed integer
     * @param  {uint8_t} reg : 
     * @return {int16_t}     : 
     */
    int16_t readS16(uint8_t reg);

    
};

#endif