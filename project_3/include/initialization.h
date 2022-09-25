#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include "constants.h"
#include "context.h"
#include <Arduino.h>

class initialization_state : public State
{
public:
	void on_do() override;

	void on_entry() override;

	void on_exit() override;

	void on_set_operational() override;

	void on_set_preoperational() override;

	void on_reset() override;
};
#endif