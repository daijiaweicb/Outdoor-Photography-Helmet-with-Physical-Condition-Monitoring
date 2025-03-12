#ifndef _MOTOR_SETTING_H
#define _MOTOR_SETTING_H

#include <iostream>
#include <gpiod.h>
#include <mutex>
#include <unistd.h> 

#define chipNo 0 

/**
 * The step motor use pin 17, 27, 22, 5 
 * Chip No is 0
 */


#define motor_pin1 17
#define motor_pin2 27
#define motor_pin3 22
#define motor_pin4 5

class StepperMotor
{
private:
    gpiod_chip *chipGPIO = nullptr;
    gpiod_line *pins[4] = {nullptr};
    int gpio_pins[4];
    int step_delay = 1000;

public:
    /**
     * @brief start the motor
     * @return {bool}  : 
     */
    bool start();
    
    /**
     * @brief motor move forward
     * @param  {int} steps : 
     */
    void forward(int steps);

    
    /**
     * @brief motor move backward
     * @param  {int} steps : 
     */
    void backward(int steps);

    
    /**
     * @brief release pins and close the chip
     */
    void cleanup();
    
    /**
     * @brief define the step for the step motor
     * @param  {int [4]} stepPattern : 
     */
    void step(int stepPattern[4]);
};

#endif
