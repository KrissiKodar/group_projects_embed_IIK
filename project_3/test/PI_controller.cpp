#include "PI_controller.h"

PI_Controller::PI_Controller(double Kp, double Ti, double integration_T)
{
    this->Kp = Kp;
    this->Ti = Ti;
    this->integration_T = integration_T;
}

double PI_Controller::update(double ref, double actual)
{
    error = ref - actual;
    sum_error += error * integration_T;
    output =  Kp * (error + sum_error/Ti);
    return output;
}