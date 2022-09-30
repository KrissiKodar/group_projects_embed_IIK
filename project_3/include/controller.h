#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <stdint.h>
#include <avr/io.h>

class Controller
{
public:
    Controller();
    void init(float proportional_gain);
    float update(float ref, float actual);
    void brake();
private:
    float error;
    float Kp;
};

#endif 

