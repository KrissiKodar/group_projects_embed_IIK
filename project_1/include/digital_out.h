#ifndef DIGITAL_OUT_H
#define DIGITAL_OUT_H
#include <stdint.h>
#include <avr/io.h>

class Digital_out
{
public:
    Digital_out(int pin);
    void init();
    void set_hi();
    void set_lo();
    void toggle();
private:
    uint8_t pin_mask;
};
#endif // DIGITAL_OUT