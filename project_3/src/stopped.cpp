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
extern volatile double reference_speed;
extern volatile double P;
extern volatile double speed;

void stopped_state::on_do()
{
  // kveikja/slokkva led 2 sinni a sekundu (2 Hz)
  if (timer1_int_count % 500 == 0)
  {
    led.toggle();
    timer1_int_count = constants::interval;
  }
}

void stopped_state::on_entry()
{
  Serial.println("light is red");
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