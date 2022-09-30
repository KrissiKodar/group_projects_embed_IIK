#ifndef P_CONTROLLER_H
#define P_CONTROLLER_H

#include "controller_test.h"

class P_Controller: public Controller_test
{
public:
    P_Controller(float Kp, const float max_output = 0.99);
    float update(float ref, float actual) override;
private:
    float error;
    float Kp;
    float output;
    float max_output;
};

#endif