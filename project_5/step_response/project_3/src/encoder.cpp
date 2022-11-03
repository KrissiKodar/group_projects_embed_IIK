#include "encoder.h" // Interface implemented in this file
#include <avr/io.h>      // Code used for the implementation
#include <avr/interrupt.h>
//#include <digital_in.h>  // Digital_in class
//#include <digital_out.h>

encoder::encoder()
{
    //
}

void encoder::init(bool enc_input1)
{
    counter = 0;
    last_input1 = enc_input1;
}

void encoder::init_interrupt()
{
    DDRD &= ~(1 << DDD2); // set the PD2 pin as input
    PORTD |= (1 << PORTD2); // enable pull-up resistor on PD2
    EICRA |= (1 << ISC00); // set INT0 to trigger on ANY logic change
    EIMSK |= (1 << INT0); // Turns on INT0

    //DDRD &= ~(1 << DDD3); // set the PD3 pin as input
    //PORTD |= (1 << PORTD3); // enable pull-up resistor on PD3
    //EICRA |= (1 << ISC01); // set INT1 to trigger on ANY logic change
    //EIMSK |= (1 << INT1); // Turns on INT1
}

int encoder::get_counter()
{
    return counter;
}

void encoder::reset_counter()
{
    counter = 0;
}

int encoder::position(bool enc_input1, bool enc_input2)
{
    bool encoder_input1_state = enc_input1;
    if (encoder_input1_state != last_input1)
    {
        if (enc_input2 != encoder_input1_state)
        {
            counter++;
        }
        else
        {
            counter--;
        }
        
        last_input1 = encoder_input1_state;
    }
    return counter;
}
