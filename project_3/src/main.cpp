#include <Arduino.h>
#include "constants.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include "constants.h"
#include "context.h"
#include "initialization.h"
#include "timer0_msec.h"
#include "timer_msec.h"
#include "timer2_msec.h"
#include "digital_out.h"
#include "digital_in.h"
#include "encoder.h"
#include "controller.h"
#include "PI_controller.h"
#include "P_controller.h"

Digital_out led(5);
Digital_out in_1(0);
Digital_out in_2(4);
Digital_out PWM_pin(1);
Digital_in encoder_input1(2);
Digital_in encoder_input2(3);
PI_Controller pi_controller(0, 1.0, 0, 0.99);
P_Controller p_controller(0, 0.99);
Controller* chosen_controller{nullptr};
encoder enc;

Timer0_msec timer0;
Timer_msec timer1;
Timer2_msec timer2;

float speed = 0.0;
int timer1_int_count = 0;
int timer1_int_count_2 = 0;
int update_time = 0.0;
float duty_cycle = 0.0;

float reference_speed = 140.0;
float error = 0.0;
float P = 1.4 / constants::max_speed;
float Ti = 0.01;
float integration_T = 0.001;
float max_output = 0.99;
float control_signal = 0.0;

int led_freq = 1;
bool cont = false;
// for storing speed and duty cycle values for the plots
//double speed_array[200];
//double duty_cycle_array[200];
//int index = 0;

Context *context;

void setup()
{
	///////// for serial monitor /////////
	Serial.begin(9600);
	// initialize internal parameters
  ////////////// for led ///////////////
	led.init();
	/////////// for encoder /////////////
	encoder_input1.init();
	encoder_input2.init();
	enc.init(encoder_input1.is_hi());

	/////////// for H-bridge /////////////
	PWM_pin.init();
	in_1.init();
	in_2.init();

	// use if encoder is connected to interrupt pins
	enc.init_interrupt();
	/////////// for timers /////////////
	timer0.init(constants::control_rate);
	timer1.init(constants::interval);// MILLISEC (max 4194.304 millisec)
	timer2.init(1500, 0.0); // MICROSEC (max 16384 microsec)
	context = new Context(new initialization_state);
}

void loop()
{
	// put your main code here, to run repeatedly:
	int command = 0;

	//delay(100);

	context->do_work();

	if (Serial.available())
	command = Serial.read();

	if (command == 'o')
	context->command_set_operational();

	if (command == 'p')
	context->command_set_preoperational();

	if (command == 'r')
	context->reset();

	if (command == 's')
	context->command_stop();
}



// interrupts at every every pulse
ISR(INT0_vect)
{
	// increment/decrement counter (counting pulses)
	enc.position(encoder_input1.is_hi(), encoder_input2.is_hi());
}

// interrupts at update rate
ISR(TIMER0_COMPA_vect)
{
	update_time += 1;
}

// interrupts at every interval ms and calculates speed
// then it resets the pulse counter
ISR(TIMER1_COMPA_vect)
{

	// counter used to print at equal time intervals
	timer1_int_count += constants::interval;
	timer1_int_count_2 += constants::interval;

	// calculate the speed
	speed = (((enc.get_counter() / constants::interval) * 1000.0) / 1400.0) * 60.0;
	
	enc.reset_counter();

	// kveikja/slokkva led x sinni a sekundu (x Hz)
	if (((timer1_int_count % (1000/led_freq)) == 0) && (cont == false))
	{
		led.toggle();
		timer1_int_count = 0;
	}

	// puts speed and duty cycle values in arrays for plotting
	/* speed_array[index] = speed;
	duty_cycle_array[index] = duty_cycle;
	index++; */
}

///////// PWM interrupts //////////
ISR(TIMER2_COMPA_vect)
{
	PWM_pin.set_hi();
}

ISR(TIMER2_COMPB_vect)
{
	PWM_pin.set_lo();
}

///////////////////////////////////

// The other external interrupt pin (not used)
// ISR (INT1_vect)
//{
//	/* interrupt handler code here */
//	Serial.print('int1 active');
//	Serial.print('\n');
//}