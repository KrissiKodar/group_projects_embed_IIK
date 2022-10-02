#include "constants.h"
#include <stdint.h>
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
#include "encoder.h"

extern Digital_out led;
extern Digital_out in_1;
extern Digital_out in_2;
extern Digital_out PWM_pin;
extern Digital_in encoder_input1;
extern Digital_in encoder_input2;
extern PI_Controller pi_controller;
extern P_Controller p_controller;
extern encoder enc;

extern Timer0_msec timer0;
extern Timer_msec timer1;
extern Timer2_msec timer2;

extern int timer1_int_count;
extern uint8_t led_freq;
extern float P;
extern float Ti;

void initialization_state::on_do()
{
 // do nothing
}

void initialization_state::on_entry()
{
  Serial.println("Initializing motor");
  pi_controller.init(0, 1.0, constants::integration_T, constants::max_output);
  p_controller.init(0, constants::max_output);
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