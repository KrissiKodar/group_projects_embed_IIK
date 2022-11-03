#ifndef CONTEXT_H
#define CONTEXT_H

#include "state.h"

class Context
{
  /**
   * @var State A reference to the current state of the Context.
   */

private:
  State *state_;

public:
  Context(State *state);
  ~Context();

  void transition_to(State *state);
  void do_work();
  void command_set_operational();
  void command_set_preoperational();
  void reset();
  void command_stop();
};

#endif