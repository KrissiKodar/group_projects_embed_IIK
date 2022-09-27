#include "global_variables.h"
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

void preoperational_state::on_do()
{
  
}

void preoperational_state::on_entry()
{

  Serial.println("/////////// Configuration /////////// ");
  // read in input from serial
  // set the input as the value for the P controller
  led_freq = 1; // Hz
  if (Serial.available() > 0)
  {
    Serial.print("Enter reference speed: ");
    reference_speed = Serial.parseFloat();
    Serial.print("Enter P value: ");
    P = Serial.parseFloat();
    // initialize the controller
    controller.init(P);
  }
  Serial.println("//////////////////////////////////// ");
}

void preoperational_state::on_exit()
{
  
}

void preoperational_state::on_set_operational()
{
  Serial.println("I received set operational command");
  this->context_->transition_to(new operational_state);
}

void preoperational_state::on_set_preoperational()
{
  Serial.println("I received set preoperational command");
  this->context_->transition_to(new preoperational_state);
}

void preoperational_state::on_reset()
{
  Serial.println("I received a reset command");
  this->context_->transition_to(new initialization_state);
}