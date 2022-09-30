#include "constants.h"
#include "stopped.h"
#include "operational.h"
#include "preoperational.h"
#include "initialization.h"
#include "digital_out.h"
#include "digital_in.h"
#include "controller.h"

extern Digital_out led;
extern Digital_out in_1;
extern Digital_out in_2;
extern Digital_out PWM_pin;



Controller::Controller()
{
}

void Controller::init(float proportional_gain)
{
    Kp = proportional_gain;
}

float Controller::update(float ref, float actual)
{
    error = ref - actual;
    return Kp * error;
}

void Controller::brake()
{
    in_1.set_hi();
	in_2.set_hi();
}