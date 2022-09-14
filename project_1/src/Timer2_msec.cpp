#include <avr/io.h>
#include "timer2_msec.h"

Timer2_msec::Timer2_msec()
{
    //2
}

void Timer2_msec::init(int period_microsec)
{
    // counting 16000000/1024 cycles of a clock prescaled by 1024
    TCCR2A = 0; // set timer2 to normal operation (all bits in control registers A and B set to zero)
    TCCR2B = 0; //
    TCNT2 = 0; // initialize counter value to 0
    OCR2A = static_cast<uint8_t>((16000000/1024 - 1)*period_microsec/1000000.0); // assign target count to compare register A (must be less than 256)
    TCCR2B |= (1 << WGM22); // clear the timer on compare match A
    TIMSK2 |= (1 << OCIE2A); // set interrupt on compare match A
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // set prescaler to 1024 and start the timer
}

void Timer2_msec::init(int period_microsec, float duty_cycle)
{
    TCCR2A = 0; // set timer2 to normal operation (all bits in control registers A and B set to zero)
    TCCR2B = 0;
    TCNT2 = 0; // initialize counter value to 0
    OCR2A = static_cast<uint8_t>((16000000/1024 - 1)*period_microsec/1000000.0); // assign target count to compare register A (must be less than 256)
    OCR2B = static_cast<uint8_t>(OCR2A * duty_cycle); // assign target count to compare register B (should not exceed value of OCR1A)
    TCCR2B |= (1 << WGM22); // clear the timer on compare match A
    TIMSK2 |= (1 << OCIE2A); // set interrupt on compare match A
    TIMSK2 |= (1 << OCIE2B); // set interrupt on compare match B
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // set prescaler to 1024 and start the timer
}

void Timer2_msec::set(float duty_cycle)
{
    OCR2B = static_cast<uint8_t>(OCR2A * duty_cycle); // assign target count to compare register B (should not exceed value of OCR1A)
}