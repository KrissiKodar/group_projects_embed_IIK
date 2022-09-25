#include "stopped.h"
#include "operational.h"
#include "preoperational.h"
#include "initialization.h"


void operational_state::on_do()
{
  
}

void operational_state::on_entry()
{
  Serial.println("light is green");
}

void operational_state::on_exit()
{
  
}

void operational_state::on_set_operational()
{
  Serial.println("I received a go command");
}

void operational_state::on_set_preoperational()
{
  Serial.println("I received a stop command");
  this->context_->transition_to(new preoperational_state);
}

void operational_state::on_reset()
{
  Serial.println("I received a reset command");
}