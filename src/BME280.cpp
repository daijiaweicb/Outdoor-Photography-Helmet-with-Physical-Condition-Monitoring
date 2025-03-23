#include "BME280.h"

void BME::beginBME()
{
    iic.iic_open(BME280_ADDRESS);
    dig_T1 = iic.readU16(0x88);
    dig_T2 = iic.readS16(0x8A);
    dig_T3 = iic.readS16(0x8C);
    dig_P1 = iic.readU16(0x8E);
    dig_P2 = iic.readS16(0x90);
    dig_P3 = iic.readS16(0x92);
    dig_P4 = iic.readS16(0x94);
    dig_P5 = iic.readS16(0x96);
    dig_P6 = iic.readS16(0x98);
    dig_P7 = iic.readS16(0x9A);
    dig_P8 = iic.readS16(0x9C);
    dig_P9 = iic.readS16(0x9E);

    iic.iic_writeRegister(0xF4, 0xFF);
    iic.iic_writeRegister(0xF5, 0x14);
    t_fine = 0;

    timer_1s.start(1000, [&]()
                   { DataReady(results); });
}

float BME::compensateTemp(int32_t adc_T)
{
    double var1 = (adc_T / 16384.0 - dig_T1 / 1024.0) * dig_T2;
    double var2 = pow(adc_T / 131072.0 - dig_T1 / 8192.0, 2) * dig_T3;
    t_fine = var1 + var2;
    return (var1 + var2) / 5120.0;
}

float BME::compensatePress(int32_t adc_P)
{
    double var1 = t_fine / 2.0 - 64000.0;
    double var2 = var1 * var1 * dig_P6 / 32768.0;
    var2 += var1 * dig_P5 * 2.0;
    var2 = var2 / 4.0 + dig_P4 * 65536.0;

    var1 = (dig_P3 * var1 * var1 / 524288.0 + dig_P2 * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * dig_P1;

    if (var1 == 0)
        return 0;

    double p = 1048576.0 - adc_P;
    p = (p - var2 / 4096.0) * 6250.0 / var1;
    var1 = dig_P9 * p * p / 2147483648.0;
    var2 = p * dig_P8 / 32768.0;
    return p + (var1 + var2 + dig_P7) / 16.0;
}

void BME::GetData(BMEresults &results)
{

    // Temperature
    uint8_t temp_msb = iic.readByte(0xFA);
    uint8_t temp_lsb = iic.readByte(0xFB);
    uint8_t temp_xlsb = iic.readByte(0xFC);
    int32_t adc_T = (temp_msb << 12) | (temp_lsb << 4) | (temp_xlsb >> 4);
    results.temp = compensateTemp(adc_T);

    // Pressure
    uint8_t press_msb = iic.readByte(0xF7);
    uint8_t press_lsb = iic.readByte(0xF8);
    uint8_t press_xlsb = iic.readByte(0xF9);
    int32_t adc_P = (press_msb << 12) | (press_lsb << 4) | (press_xlsb >> 4);
    results.press = compensatePress(adc_P);
}

void BME::DataReady(BMEresults &results)
{
    for (auto &cb : BMEcallbackinterface)
    {
        GetData(results);
        cb->BMECallback(results);
    }
}

void BME::BMERigister(Callbackinterface *ci)
{
    BMEcallbackinterface.push_back(ci);
}