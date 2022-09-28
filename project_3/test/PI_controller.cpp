#include "PI_controller.h"

PI_Controller::PI_Controller()
{
}

void PI_Controller::init(double proportional_gain)
{
    Kp = proportional_gain;
}

double PI_Controller::update(double ref, double actual)
{
    error = ref - actual;
    return Kp * error;
}