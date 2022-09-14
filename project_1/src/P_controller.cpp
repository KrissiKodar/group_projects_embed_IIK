#include "p_controller.h"

P_controller::P_controller()
{
}

void P_controller::init(double proportional_gain)
{
    Kp = proportional_gain;
}

double P_controller::update(double ref, double actual)
{
    error = ref - actual;
    return Kp * error;
}