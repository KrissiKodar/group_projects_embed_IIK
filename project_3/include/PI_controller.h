#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H

#include "controller_test.h"

class PI_Controller: public Controller_test
{
public:
    PI_Controller(double Kp, double Ti, double integration_T, double max_output = 0.99);
    void init(double Kp, double Ti, double integration_T, double max_output = 0.99);
    double update(double ref, double actual) override;
    void brake() override;
private:
    double error;
    double Kp;
    double Ti;
    double integration_T;
    double max_output = 1.0;
    double sum_error = 0;
    double output = 0;
};

#endif