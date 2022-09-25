#ifndef FORWARD_DECLARE_OBJECTS_H
#define FORWARD_DECLARE_OBJECTS_H

#include "timer0_msec.h"
#include "timer_msec.h"
#include "timer2_msec.h"
#include "digital_out.h"
#include "digital_in.h"
#include "encoder.h"
#include "controller.h"

Digital_out led(5);
Digital_out in_1(0);
Digital_out in_2(4);
Digital_out PWM_pin(1);
Digital_in encoder_input1(2);
Digital_in encoder_input2(3);
Controller controller;
encoder enc;

Timer0_msec timer0;
Timer_msec timer1;
Timer2_msec timer2;

#endif