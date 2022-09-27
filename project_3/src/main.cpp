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

Digital_out led(5);
Digital_out in_1(0);
Digital_out in_2(4);
Digital_out PWM_pin(1);
Digital_in encoder_input1(2);
Digital_in encoder_input2(3);
Controller controller;
encoder enc;

Timer0_msec timer0;
Timer_msec timer1;
Timer2_msec timer2;

volatile double speed = 0.0;
double timer1_count = 0.0;
volatile int timer1_int_count = 0;
volatile float duty_cycle = 0.0;


volatile double reference_speed = 140.0;
volatile double error = 0.0;
volatile double P = 1.4 / constants::max_speed;
volatile double control_signal = 0.0;

// for storing speed and duty cycle values for the plots
//volatile double speed_array[200];
//volatile double duty_cycle_array[200];
//volatile int index = 0;

Context *context;

void setup()
{
	///////// for serial monitor /////////
	Serial.begin(9600);

	////////////// for led ///////////////
	led.init();

	/////////// for H-bridge /////////////
	PWM_pin.init();
	in_1.init();
	in_2.init();
	in_1.set_lo();
	in_2.set_hi();

	/////////// for encoder /////////////
	encoder_input1.init();
	encoder_input2.init();
	enc.init(encoder_input1.is_hi());

	// use if encoder is connected to interrupt pins
	enc.init_interrupt();

	/////////// for timers /////////////
	// find out what frequency is needed for PWM
	// timer0.init((1.0/control_rate)*1000000.0); //MICROSEC (max 16384 microsec)
	timer0.init(constants::control_rate);
	timer1.init(constants::interval);		   // MILLISEC (max 4194.304 millisec)
	timer2.init(1500, duty_cycle); // MICROSEC (max 16384 microsec)

	
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
	context->reset();
}
// interrupts at every every pulse
ISR(INT0_vect)
{
	// increment/decrement counter (counting pulses)
	enc.position(encoder_input1.is_hi(), encoder_input2.is_hi());

	// for measuring time constant
	// GO = true;
}

// interrupts at update rate
ISR(TIMER0_COMPA_vect)
{
	// Part 3 P control
	// control output should be updated at a mininum rate
	// set speed to x pulses per second
	// Update the duty cycle value
	duty_cycle = controller.update(reference_speed, speed);
	
	// if the duty cycle is greater than 100%, set it to 100%
	// if the duty cycle is less than 0%, set it to 0%
	if (duty_cycle >= 1.0)
	{
		duty_cycle = 0.99;
	}
	else if (duty_cycle < 0.0)
	{
		duty_cycle = 0.0;
	}
	// set the duty cycle
	timer2.set(duty_cycle);
}

// interrupts at every interval ms and calculates speed
// then it resets the pulse counter
ISR(TIMER1_COMPA_vect)
{

	// counter used to print at equal time intervals
	timer1_int_count += constants::interval;

	// calculate the speed
	speed = (((enc.get_counter() / constants::interval) * 1000.0) / 1400.0) * 60.0;
	
	enc.reset_counter();

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