#include "P_controller.h"
#include "digital_out.h"

extern Digital_out led;
extern Digital_out in_1;
extern Digital_out in_2;
extern Digital_out PWM_pin;

P_Controller::P_Controller(float Kp, float max_output)
{
    this->Kp = Kp;
    this->max_output = max_output;
}

void P_Controller::init(float Kp, float max_output)
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

void P_Controller::brake()
{
    in_1.set_hi();
	in_2.set_hi();
}