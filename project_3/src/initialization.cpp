#include "constants.h"
#include "stopped.h"
#include "operational.h"
#include "preoperational.h"
#include "initialization.h"
#include "digital_out.h"
#include "digital_in.h"
#include "PI_controller.h"
#include "P_controller.h"
#include "timer0_msec.h"
#include "timer_msec.h"
#include "timer2_msec.h"

extern Digital_out led;
extern Digital_out in_1;
extern Digital_out in_2;
extern Digital_out PWM_pin;
extern PI_Controller controller;

extern Timer0_msec timer0;
extern Timer_msec timer1;
extern Timer2_msec timer2;

extern volatile int timer1_int_count;
extern volatile int led_freq;

void initialization_state::on_do()
{
 // do nothing
}

void initialization_state::on_entry()
{
  Serial.println("Initializing motor");
  // initialize internal parameters
  
	/////////// for H-bridge /////////////
	in_1.set_lo();
	in_2.set_hi();
	timer2.init(1500, 0.0); // MICROSEC (max 16384 microsec)

  // enable interrupts
  sei();
  delay(2000);
  // when the timer expires, transition to red
  Serial.println("Motor initialized, ready to receive commands.");
  this->context_->transition_to(new preoperational_state);
}

void initialization_state::on_exit()
{
  
}

void initialization_state::on_set_operational()
{
  // do nothing
}

void initialization_state::on_set_preoperational()
{
  // do nothing
}

void initialization_state::on_reset()
{
  // do nothing
}
void initialization_state::on_stop()
{
  //d o nothing
}