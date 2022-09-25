#include "global_variables.h"
#include "stopped.h"
#include "operational.h"
#include "preoperational.h"
#include "initialization.h"


void preoperational_state::on_do()
{
}

void preoperational_state::on_entry()
{

  Serial.println("/////////// Configuration /////////// ");
  // read in input from serial
  // set the input as the value for the P controller
  if (Serial.available() > 0)
  {
    Serial.print("SEnter reference speed: ");
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
  Serial.println("I received a go command");
}

void preoperational_state::on_set_preoperational()
{
  Serial.println("I received a stop command");
}

void preoperational_state::on_reset()
{
  Serial.println("I received a reset command");
}