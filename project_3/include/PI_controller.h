#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H

#include "controller.h"

class PI_Controller: public Controller
{
public:
    PI_Controller(float Kp, float Ti, float integration_T, float max_output);
    void init(float Kp, float Ti, float integration_T, float max_output);
    float update(float ref, float actual) override;
    void brake() override;
private:
    float error;
    float Kp;
    float Ti;
    float integration_T;
    float max_output = 1.0;
    float sum_error = 0;
    float output = 0;
};

#endif