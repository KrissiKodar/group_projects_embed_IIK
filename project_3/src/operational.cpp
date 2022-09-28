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
extern volatile int timer1_int_count_2;
extern volatile int led_freq;
extern volatile bool cont;
extern volatile double reference_speed;
extern volatile double P;
extern volatile double speed;

void operational_state::on_do()
{
  if ((timer1_int_count_2 % 1000) == 0)
	{
		// print out the reference speed and the actual speed in RPM
    Serial.print("Reference speed: ");
    Serial.print(reference_speed);
    Serial.print(" Actual speed: ");
    Serial.println(speed);
		timer1_int_count_2 = constants::interval;
	} 
}

void operational_state::on_entry()
{
  // ligh continuously on
  cont = true;
  led.set_hi();
  controller.init(P);
  Serial.println("/////////// Motor is in operational mode /////////// ");
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

void operational_state::on_stop()
{
  Serial.println("I received a stop command");
  this->context_->transition_to(new stopped_state);
}