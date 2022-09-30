#include "PI_controller.h"
#include "digital_out.h"


extern Digital_out led;
extern Digital_out in_1;
extern Digital_out in_2;
extern Digital_out PWM_pin;

PI_Controller::PI_Controller(float Kp, float Ti, float integration_T, float max_output)
{
    this->Kp = Kp;
    this->Ti = Ti;
    this->integration_T = integration_T;
    this->max_output = max_output;
    sum_error = 0;
}

void PI_Controller::init(float Kp, float Ti, float integration_T, float max_output)
{
    this->Kp = Kp;
    this->Ti = Ti;
    this->integration_T = integration_T;
    this->max_output = max_output;
    sum_error = 0;
}

float PI_Controller::update(float ref, float actual)
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