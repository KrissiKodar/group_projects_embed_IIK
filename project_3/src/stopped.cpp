#include "constants.h"
#include "stopped.h"
#include "operational.h"
#include "preoperational.h"
#include "initialization.h"
#include "digital_out.h"
#include "digital_in.h"
#include "PI_controller.h"

extern Digital_out led;
extern Digital_out in_1;
extern Digital_out in_2;
extern Digital_out PWM_pin;
extern Controller* chosen_controller;

extern int timer1_int_count;
extern int led_freq;
extern bool cont;

void stopped_state::on_do()
{
}

void stopped_state::on_entry()
{
  (*chosen_controller).brake();
  cont = false;
  led_freq = 2;
  Serial.println("/////////// Motor is in stopped mode /////////// ");
}

void stopped_state::on_exit()
{
  
}

void stopped_state::on_set_operational()
{
  Serial.println("I received set operational command");
  this->context_->transition_to(new operational_state);
}

void stopped_state::on_set_preoperational()
{
  Serial.println("I received set operational command");
  this->context_->transition_to(new preoperational_state);
}

void stopped_state::on_reset()
{
  Serial.println("I received a reset command");
  this->context_->transition_to(new initialization_state);
}

void stopped_state::on_stop()
{
  // do nothing
}