#ifndef BME280_H
#define BME280_H

#include "iic.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstdint>
#include <cmath>


#include <memory>
#include "Event_callback.h"
#include <thread>

#define BMP280_ADDRESS 0x76
#define BMP280_ID 0x58

class BMP{
private:

    int fd;
    //Register data
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2, dig_P3, dig_P4, dig_P5;
    int16_t dig_P6, dig_P7, dig_P8, dig_P9;


    int32_t t_fine;

    //Read from register 
    uint8_t readByte(uint8_t reg) {
        uint8_t value;
        if (write(fd, &reg, 1) != 1) throw "register read successful";
        if (read(fd, &value, 1) != 1) throw "fail to read register";
        return value;
    }


    uint16_t readU16(uint8_t reg) {
        uint8_t buf[2];
        if (write(fd, &reg, 1) != 1) throw "register read successful";
        if (read(fd, buf, 2) != 2) throw "fail to read register";
        return (buf[1] << 8) | buf[0];
    }

    int16_t readS16(uint8_t reg) {
        uint16_t val = readU16(reg);
        return (val > 32767) ? val - 65536 : val;
    }

    void writeByte(uint8_t reg, uint8_t value) {
        uint8_t buf[2] = {reg, value};
        if (write(fd, buf, 2) != 2) throw "Fail to set up";
    }

    void loadCalibration() {
        dig_T1 = readU16(0x88);
        dig_T2 = readS16(0x8A);
        dig_T3 = readS16(0x8C);
        
        dig_P1 = readU16(0x8E);
        dig_P2 = readS16(0x90);
        dig_P3 = readS16(0x92);
        dig_P4 = readS16(0x94);
        dig_P5 = readS16(0x96);
        dig_P6 = readS16(0x98);
        dig_P7 = readS16(0x9A);
        dig_P8 = readS16(0x9C);
        dig_P9 = readS16(0x9E);
    }

    void beginBMP();

    void RegisterSetting(std::shared_ptr<CallbackInterface> cb);


public:
    
    BMP(const char* device = "/dev/i2c-1") {
        if ((fd = open(device, O_RDWR)) < 0)
            throw "iic fail";
            
        if (ioctl(fd, I2C_SLAVE, BMP280_ADDRESS) < 0)
            throw "iic fail";
            
        
            
        loadCalibration();
        
        
        writeByte(0xF4, 0xFF);
    
        writeByte(0xF5, 0x14);
        
        t_fine = 0;
    }

    ~BMP() {
        close(fd);
    }

    //Data caclulation for temperature

    float compensateTemp(int32_t adc_T) {
        double var1 = (adc_T/16384.0 - dig_T1/1024.0) * dig_T2;
        double var2 = pow(adc_T/131072.0 - dig_T1/8192.0, 2) * dig_T3;
        t_fine = var1 + var2;
        return (var1 + var2) / 5120.0;
    }

    float compensatePress(int32_t adc_P) {
        double var1 = t_fine/2.0 - 64000.0;
        double var2 = var1 * var1 * dig_P6 / 32768.0;
        var2 += var1 * dig_P5 * 2.0;
        var2 = var2/4.0 + dig_P4*65536.0;
        
        var1 = (dig_P3 * var1 * var1/524288.0 + dig_P2 * var1) / 524288.0;
        var1 = (1.0 + var1/32768.0) * dig_P1;
        
        if (var1 == 0) return 0;
        
        double p = 1048576.0 - adc_P;
        p = (p - var2/4096.0) * 6250.0 / var1;
        var1 = dig_P9 * p * p / 2147483648.0;
        var2 = p * dig_P8 / 32768.0;
        return p + (var1 + var2 + dig_P7)/16.0;
    }
        
    void getData(float &temp, float &press) {
        
        //Temperature
        uint8_t temp_msb = readByte(0xFA);
        uint8_t temp_lsb = readByte(0xFB);
        uint8_t temp_xlsb = readByte(0xFC);
        int32_t adc_T = (temp_msb << 12) | (temp_lsb << 4) | (temp_xlsb >> 4);
        temp = compensateTemp(adc_T);

        //Pressure    
        uint8_t press_msb = readByte(0xF7);
        uint8_t press_lsb = readByte(0xF8);
        uint8_t press_xlsb = readByte(0xF9);
        int32_t adc_P = (press_msb << 12) | (press_lsb << 4) | (press_xlsb >> 4);
        press = compensatePress(adc_P);
    }



    std::shared_ptr<CallbackInterface> callback;
    


};





#endif