#include <avr/io.h>

#include "digital_out.h"

Digital_out::Digital_out(int pin)
{
    pin_mask = (1 << pin);
}

void Digital_out::init()
{
    DDRB |= pin_mask;
}

void Digital_out::set_hi()
{
    PORTB |= pin_mask;
}

void Digital_out::set_lo()
{
    PORTB &= ~pin_mask;
}

void Digital_out::toggle()
{
    PORTB = PORTB ^ pin_mask;
}