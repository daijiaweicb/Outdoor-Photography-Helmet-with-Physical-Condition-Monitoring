#include <iostream>
#include "BME280.h"

class BMERealization : public BME,public BME::Callbackinterface
{
    public:
    void BMECallback(BMEresults &results) override
    {
        
    }
};


int main()
{

    return 0;
}