#include "constants.h"
#include "stopped.h"
#include "operational.h"
#include "preoperational.h"
#include "initialization.h"
#include "digital_out.h"
#include "digital_in.h"
#include "PI_controller.h"
#include "P_controller.h"

extern Digital_out led;
extern Digital_out in_1;
extern Digital_out in_2;
extern Digital_out PWM_pin;
extern PI_Controller pi_controller;
extern P_Controller p_controller;
extern Controller* chosen_controller;

extern int timer1_int_count;
extern int led_freq;
extern bool cont;
extern float reference_speed;
extern float P;
extern float Ti;
extern float integration_T;
extern float max_output;

void preoperational_state::on_do()
{
  
}

void preoperational_state::on_entry()
{

  Serial.println("/////////// Configuration /////////// ");
  // read in input from serial
  // set the input as the value for the P controller
  int command = 0;
  cont = false;
  led_freq = 1; // Hz
  while (Serial.available() == 0)
  {
    Serial.println("Enter reference speed: ");
    reference_speed = Serial.parseFloat();
    // print new line
    Serial.println("Enter K_p value: ");
    P = Serial.parseFloat()/constants::max_speed;
    Serial.println("Enter T_i value: ");
    Ti = Serial.parseFloat();
    Serial.println("Press 1 for P-controller");
    Serial.println("Press 2 for PI-controller");
    command = Serial.parseInt();
    if (command == 1)
    {
      Serial.println("P-controller selected");
      p_controller.init(P,max_output);
      chosen_controller = &p_controller;
    }
    else if (command == 2)
    {
      Serial.println("PI-controller selected");
      pi_controller.init(P, Ti, integration_T, max_output);
      chosen_controller = &pi_controller;
    }
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

void preoperational_state::on_stop()
{
  Serial.println("I received a stop command");
  this->context_->transition_to(new stopped_state);
}