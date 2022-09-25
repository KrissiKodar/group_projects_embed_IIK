#include "constants.h"
#include "stopped.h"
#include "operational.h"
#include "preoperational.h"
#include "initialization.h"

void stopped_state::on_do()
{
  
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
  Serial.println("I received a go command");
  this->context_->transition_to(new operational_state);
}

void stopped_state::on_set_preoperational()
{
  Serial.println("I received a stop command");
}

void stopped_state::on_reset()
{
  Serial.println("I received a reset command");
}