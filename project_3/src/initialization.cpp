#include "constants.h"
#include "stopped.h"
#include "operational.h"
#include "preoperational.h"
#include "initialization.h"

void initialization_state::on_do()
{
  // do nothing
}

void initialization_state::on_entry()
{
  Serial.println("Initializing motor");
  // initialize internal parameters
  sei();
  delay(2000);
  // when the timer expires, transition to red
  this->context_->transition_to(new preoperational_state);
}

void initialization_state::on_exit()
{
  
}

void initialization_state::on_set_operational()
{
  // do nothing
}

void initialization_state::on_set_preoperational()
{
  // do nothing
}

void initialization_state::on_reset()
{
  // do nothing
}