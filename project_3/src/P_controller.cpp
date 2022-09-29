#include "P_controller.h"

P_Controller::P_Controller(double Kp, double max_output = 0.99)
{
    this->Kp = Kp;
    this->max_output = max_output;
}

double P_Controller::update(double ref, double actual)
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