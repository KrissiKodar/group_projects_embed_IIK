#include "motor_driver.h"
#include "digital_out.h"
#include "timer2_msec.h"

extern float duty_cycle;
//extern float speed;
//extern float reference_speed;
extern int& reference_speed;
extern float speed;
extern Digital_out in_1;
extern Digital_out in_2;
extern Timer2_msec timer2;

void motor_driver(Controller* cont)
{
	// scale because register is unsigned
	// refrence speed comes from the register on the interval [0,280]
	// but the actual speed is on the interval [-140,140]
	float reference_speed_scaled = reference_speed - 140;
    duty_cycle = cont->update(reference_speed_scaled, speed);
	// CW
	if (duty_cycle < 0)
	{
		in_1.set_hi();
		in_2.set_lo();
        duty_cycle = -duty_cycle;
	}
	else // CCW
	{
		in_1.set_lo();
		in_2.set_hi();
	}
	//duty_cycle = abs(duty_cycle);
	// set the duty cycle
	timer2.set(duty_cycle);
}