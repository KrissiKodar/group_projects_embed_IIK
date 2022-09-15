#include <digital_out.h>
#include <digital_in.h>
#include <timer0_msec.h>
#include <timer_msec.h>
#include <timer2_msec.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <encoder.h>
#include <p_controller.h>
#include <Arduino.h>

Digital_out in_1(0);
Digital_out in_2(4);
Digital_out PWM_pin(1);
Digital_in encoder_input1(2);
Digital_in encoder_input2(3);
P_controller controller;
encoder enc;

// we most likely need a second timer for the
Timer0_msec timer0;
Timer_msec timer1;
Timer2_msec timer2;

int old_counter = 0;
double deg = 0.0;
volatile double interval = 20.0;
volatile double speed = 0.0;
volatile double max_speed = 140.0; // need to find this value RPM
bool check = true;
bool GO = false;
double time_constant = 100.0; // need to find this value ms
double update_rate = 10.0/time_constant;

double timer1_count = 0.0;
volatile int timer1_int_count = 0;
volatile float duty_cycle = 0.0;

volatile double control_rate = (10.0/(time_constant/1000.0))*2.0;
volatile double set_speed = 100.0;
volatile double error = 0.0;
volatile double P = 2.0/max_speed;
volatile double control_signal = 0.0;
volatile double speed_array[100];

int main()
{
	///////// for serial monitor /////////
	Serial.begin(9600);

	/////////// for H-bridge /////////////
	PWM_pin.init();
	in_1.init();
	in_2.init();
	in_1.set_lo();
	in_2.set_hi();

	//PWM_pin.init();

	/////////// for encoder /////////////
	encoder_input1.init();
	encoder_input2.init();
	enc.init(encoder_input1.is_hi());
	// use if encoder is connected to interrupt pins
	enc.init_interrupt();

	/////////// for timer /////////////
	// find out what frequency is needed for PWM
	//timer0.init((1.0/control_rate)*1000000.0); //MICROSEC (max 16384 microsec)
	timer0.init(4000);
	timer1.init(interval); //MILLISEC (max 4194.304 millisec)
	timer2.init(1500, duty_cycle); //MICROSEC (max 16384 microsec)
	
	//timer1.init(interval,duty_cycle);
	
	// for controller
	controller.init(P);
	sei(); // enable interrupts

	while(true)
	{	
		// if timer1_int_count % 1000 == 0
		// then print out the speed
		if(timer1_int_count % 1000 == 0)
		{
			timer1_int_count = 20;
			Serial.print("Reference value: ");
			Serial.print(set_speed);
			Serial.println(" RPM");
			Serial.print("Speed: ");
			Serial.print(speed);
			Serial.println(" RPM");
			Serial.print("duty cycle: ");
			Serial.print(duty_cycle);
			Serial.println();
			Serial.println();
		}
		// Part 2 measure maximum motor speed
		/* if (speed >= max_speed)
		{
			speed = max_speed;
			Serial.println("Maximum speed: ");
			Serial.println(speed);
		} */
		// Part 2 measure time constant
		// measure time it takes to reach 63% of max speed
		// the max speed needs to  be measured first !!!!
		/* if (speed >= 0.63*max_speed && check)
		{
			check = false;
			Serial.println("Time constant: ");
			Serial.println(timer1_count);
		} */
		
		// Part 3 P control
		// control output should be updated at a mininum rate
		// set speed to x pulses per second
		// calculate error
		//error = set_speed - speed;
		//duty_cycle = P*error;
		//timer1.set(duty_cycle);
	}
}

ISR (INT0_vect)
{
	// increment/decrement counter
	enc.position(encoder_input1.is_hi(), encoder_input2.is_hi());
	//GO = true;
}

ISR(TIMER0_COMPA_vect)
{
	// Part 3 P control
	// control output should be updated at a mininum rate
	// set speed to x pulses per second
	// calculate error
	duty_cycle = controller.update(set_speed, speed);
	//duty_cycle = control_signal/max_speed;
	if (duty_cycle >= 1.0)
	{
		duty_cycle = 0.99;
	}
	else if (duty_cycle <= 0.0)
	{
		duty_cycle = 0.0;
	}
	timer2.set(duty_cycle);
}

ISR(TIMER1_COMPA_vect)
{
	// interrupts every interval ms
	// speed -> pulses per ms ---> pulses per second
	//speed = (enc.get_counter()/interval)*1000.0;
	// or in RPM
	/* if (GO == true)
	{
		timer1_count += interval;
	} */
	timer1_int_count += interval;
	speed = (((enc.get_counter()/interval)*1000.0)/1400.0)*60.0;
	// 
	enc.reset_counter();
	/* if (speed >= 0.63*max_speed && check)
		{
			Serial.println("Time constant: ");
			Serial.println(timer1_count);
			check = false;
		} */
	//Serial.println(speed);
	
}

ISR(TIMER2_COMPA_vect)
{
	// for PWM
	PWM_pin.set_hi();
}
//PWM
ISR(TIMER2_COMPB_vect)
{
	PWM_pin.set_lo();
}

// The other interrupt pin
//ISR (INT1_vect)
//{
//	/* interrupt handler code here */
//	Serial.print('int1 active');
//	Serial.print('\n');	
//}

