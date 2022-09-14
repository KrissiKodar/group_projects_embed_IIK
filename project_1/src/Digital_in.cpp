#include "digital_in.h" // Interface implemented in this file
#include <avr/io.h>      // Code used for the implementation

Digital_in::Digital_in(int pin)
{
     pin_mask = (1 << pin);
}

void Digital_in::init()
{
    DDRB &= ~ pin_mask;
    PORTB |=  pin_mask;  // Enable pull-up resistor
}

bool Digital_in::is_hi()
{
    return PINB &  pin_mask;
}

bool Digital_in::is_lo()
{
    return !is_hi();
}