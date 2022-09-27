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


void operational_state::on_do()
{
  // every 1 second print the speed
  if (timer1_int_count % 1000 == 0)
  {
    // print the speed along with the units, RPN
    Serial.print(speed);
    Serial.println(" RPM");
    timer1_int_count = constants::interval;
  }
}

void operational_state::on_entry()
{
  led.set_hi();
  // initialize the controller
  controller.init(P);
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
  controller.init(0);
  Serial.println("I received a reset command");
  this->context_->transition_to(new initialization_state);
}

void operational_state::on_stop()
{
  Serial.println("I received a stop command");
  this->context_->transition_to(new stopped_state);
}