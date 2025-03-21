#ifndef BME280_H
#define BME280_H

#include "iic.h"
#include <cstdint>
#include <cmath>
#include <memory>
#include <vector>
#include "Event_callback.h"
#include <thread>

#define BME280_ADDRESS 0x76
#define BME280_ID 0x58

class BME
{
private:
    IIC iic;
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2, dig_P3, dig_P4, dig_P5;
    int16_t dig_P6, dig_P7, dig_P8, dig_P9;

    int32_t t_fine;

    void beginBME();

    void RegisterSetting(std::shared_ptr<CallbackInterface> cb);

public:
    BME() : iic(1)
    {
    }

    ~BME()
    {
        iic.iic_close();
    }

    // Data caclulation for temperature
    struct BMEresults
    {
        float temp;
        float press;
    };

    float compensateTemp(int32_t adc_T);

    float compensatePress(int32_t adc_P);

    void getData(BMEresults &results);

    struct Callbackinterface
    {
        virtual void BMECallback(BMEresults &results) =0;
        virtual ~Callbackinterface() = default;
    };

    std::vector<Callbackinterface*> BMEcallbackinterface;

    void BMERigister(Callbackinterface* ci);
};

#endif