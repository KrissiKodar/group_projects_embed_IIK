#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <stdint.h>
#include <avr/io.h>

class Controller
{
public:
    Controller();
    void init(double proportional_gain);
    double update(double ref, double actual);
    void brake();
private:
    double error;
    double Kp;
};

#endif 

