#include "P_controller.h"

P_Controller::P_Controller(float Kp, float max_output = 0.99)
{
    this->Kp = Kp;
    this->max_output = max_output;
}

float P_Controller::update(float ref, float actual)
{
    error = ref - actual;
    output = Kp * error;
    if (output >= max_output)
    {
        output = max_output;
    }
    else if (output <= -max_output)
    {
        output = -max_output;
    }
    return output;
}