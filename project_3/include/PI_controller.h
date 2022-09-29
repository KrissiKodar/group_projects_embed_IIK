#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H

#include "controller_test.h"

class PI_Controller: public Controller_test
{
public:
    PI_Controller(double Kp, double Ti, double integration_T);
    double update(double ref, double actual) override;
private:
    double error;
    double Kp;
    double Ti;
    double integration_T;
    double sum_error;
    double output;
};

#endif