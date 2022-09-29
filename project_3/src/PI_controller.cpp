#include "PI_controller.h"
#include "digital_out.h"


extern Digital_out led;
extern Digital_out in_1;
extern Digital_out in_2;
extern Digital_out PWM_pin;

PI_Controller::PI_Controller(double Kp, double Ti, double integration_T, double max_output = 0.99)
{
    this->Kp = Kp;
    this->Ti = Ti;
    this->integration_T = integration_T;
    this->max_output = max_output;
}

void PI_Controller::init(double Kp, double Ti, double integration_T, double max_output = 0.99)
{
    this->Kp = Kp;
    this->Ti = Ti;
    this->integration_T = integration_T;
    this->max_output = max_output;
}

double PI_Controller::update(double ref, double actual)
{
    error = ref - actual;
    sum_error += error * integration_T;
    output =  Kp * (error + sum_error/Ti);
    // if output is saturated, do not integrate
    // output is saturated if it is smaller than -1 or larger than 1
    if (output >= max_output)
    {
        output = max_output;
        sum_error -= error * integration_T;
    }
    else if (output <= -max_output)
    {
        output = -max_output;
        sum_error -= error * integration_T;
    }
    return output;
}

void PI_Controller::brake()
{
    in_1.set_hi();
	in_2.set_hi();
}