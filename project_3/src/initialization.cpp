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
extern Controller controller;

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
  sei();
  delay(2000);
  // when the timer expires, transition to red
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