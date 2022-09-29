#ifndef P_CONTROLLER_H
#define P_CONTROLLER_H

#include "controller_test.h"

class P_Controller: public Controller_test
{
public:
    P_Controller(double Kp, double max_output = 0.99);
    double update(double ref, double actual) override;
private:
    double error;
    double Kp;
    double output;
    double max_output;
};

#endif