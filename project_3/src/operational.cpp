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

void operational_state::on_do()
{

}

void operational_state::on_entry()
{
  // ligh continuously on
  led_freq = constants::interval + 1.0;
  led.set_hi();
}

void operational_state::on_exit()
{
  
}

void operational_state::on_set_operational()
{
  Serial.println("I received set operational command");
  this->context_->transition_to(new operational_state);
}

void operational_state::on_set_preoperational()
{
  Serial.println("I received set preoperational command");
  this->context_->transition_to(new preoperational_state);
}

void operational_state::on_reset()
{
  Serial.println("I received a reset command");
  this->context_->transition_to(new initialization_state);
}