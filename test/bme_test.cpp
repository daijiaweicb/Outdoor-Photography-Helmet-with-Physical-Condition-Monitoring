#include <iostream>
#include "BME280.h"

class BMERealization : public BME, public BME::Callbackinterface
{
public:
    void BMECallback(BMEresults &results) override
    {
        std::cout << "temp is: " << results.temp << " pressure is: " << results.press << std::endl;
    }
};

int main()
{
    BME bme;
    BMERealization bmereal;

    bme.BMERigister(&bmereal);
    bme.beginBME();

    return 0;
}