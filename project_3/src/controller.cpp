#include "controller.h"

Controller::Controller()
{
}

void Controller::init(double proportional_gain)
{
    Kp = proportional_gain;
}

double Controller::update(double ref, double actual)
{
    error = ref - actual;
    return Kp * error;
}

void Controller::brake()
{
}