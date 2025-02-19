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

class IIC {
public:
    /**
     * construct of the class
     */
    IIC(int adapter) : adapter_nr(adapter), file(-1) {}

    
    /**
     * Open the I2C device
     * In this case, open the MPU 6050
     */
    void iic_open();

    
    /**
     * Close the I2C device
     */
    void iic_close();

    
    /**
     * 
     * @param  {uint8_t} reg     : 
     * @param  {uint8_t*} buffer : 
     * @param  {size_t} length   : 
     * @return {bool}            : 
     */
    bool readRegisters(uint8_t reg, uint8_t* buffer, size_t length);
    /**
     * 
     * @param  {uint8_t} reg   : 
     * @param  {uint8_t} value : 
     */
    void iic_writeRegister(uint8_t reg, uint8_t value);
    int file;

private:
    int adapter_nr;
    char filename[20];
};


#endif