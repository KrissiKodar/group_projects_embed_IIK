#include <digital_out.h>
#include <digital_in.h>
#include <timer_msec.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <encoder.h>
#include <Arduino.h>

Digital_out led(5);
Digital_in encoder_input1(2);
Digital_in encoder_input2(3);
encoder enc;

//Timer_msec timer1;


int old_counter = 0;
double deg = 0.0;
//double interval = 100.0;

// TEST!!!!

int main()
{
	///////// for serial monitor /////////
	Serial.begin(9600);

	led.init();
	/////////// for encoder /////////////
	encoder_input1.init();
	encoder_input2.init();
	enc.init(encoder_input1.is_hi());

	//timer1.init(interval);

	// use if encoder is connected to interrupt pins
	enc.init_interrupt();
	sei(); // enable interrupts

	while(true)
	{	
		/* 
		// if not using interrupts
		enc.position(encoder_input1.is_hi(), encoder_input2.is_hi());
		if (enc.get_counter() > old_counter)
		{
			led.set_hi();
		}
		else {
			led.set_lo();
		}
		// for degrees of output shaft
		//deg = enc.get_counter()*360.0/1400.0;
		//Serial.println(deg);
		Serial.println(enc.get_counter());
		old_counter = enc.get_counter(); */
	}
}

ISR (INT0_vect)
{
	// increment/decrement counter
	enc.position(encoder_input1.is_hi(), encoder_input2.is_hi());
	// led on arduino board turns on when motor goes clockwise
	// led on arduino board turns off when motor goes counter-clockwise
	if (enc.get_counter() > old_counter)
	{
		led.set_hi();
	}
	else
	{
		led.set_lo();
	}
	old_counter = enc.get_counter();
	// printing each interrupt can make encodeer miss pulses!
	Serial.println(enc.get_counter());
}

/* ISR(TIMER1_COMPA_vect)
{
	// interrupts every interval ms
	Serial.println(enc.get_counter());	
}
 */