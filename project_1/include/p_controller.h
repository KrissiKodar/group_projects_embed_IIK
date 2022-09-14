#ifndef P_CONTROLLER_H
#define P_CONTROLLER_H
#include <stdint.h>
#include <avr/io.h>

class P_controller
{
public:
    P_controller();
    void init(double proportional_gain);
    double update(double ref, double actual);
private:
    double error;
    double Kp;
};

#endif 

