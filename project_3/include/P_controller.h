#ifndef P_CONTROLLER_H
#define P_CONTROLLER_H

#include "controller.h"

class P_Controller: public Controller
{
public:
    P_Controller(float Kp, const float max_output);
    void init(float Kp, float max_output);
    float update(float ref, float actual) override;
    void brake() override;
private:
    float error;
    float Kp;
    float output;
    float max_output;
};

#endif