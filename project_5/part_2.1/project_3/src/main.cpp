#include <Arduino.h>
#include <stdint.h>
#include "constants.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include "constants.h"
#include "context.h"
#include "initialization.h"
#include "timer0_msec.h"
#include "timer_msec.h"
#include "timer2_msec.h"
#include "digital_out.h"
#include "digital_in.h"
#include "encoder.h"
#include "controller.h"
#include "PI_controller.h"
#include "P_controller.h"

Digital_out led(5);
Digital_out in_1(0);
Digital_out in_2(4);
Digital_out PWM_pin(1);
Digital_in encoder_input1(2);
Digital_in encoder_input2(3);
PI_Controller pi_controller;
P_Controller p_controller;
Controller *chosen_controller{nullptr};
encoder enc;

Timer0_msec timer0;
Timer_msec timer1;
Timer2_msec timer2;

// char buffer[100];

const size_t MSG_LEN = 8;
uint8_t msg[MSG_LEN];

int reg[3] = {0, 0, 0}; // {commands, speed, reference speed}
const size_t REG_LEN = sizeof(reg) /sizeof(reg[0]);

uint8_t server_address;
uint8_t function_code;
uint16_t register_address;
uint16_t register_value;
uint16_t total_to_read;

uint16_t crc_received; 
uint16_t crc_calculated;
uint16_t crc_send;

int &command = reg[0];

int &speed_reg = reg[1];

int &reference_speed = reg[2];

int last_command = 0;

int timer1_int_count = 0;
uint16_t timer1_int_count_2 = 0;
uint8_t update_time = 0.0;
float duty_cycle = 0.0;
float speed = 0.0;
float error = 0.0;
float P = 1.8 / constants::max_speed;
float Ti = 0.048;

uint8_t led_freq = 1;
bool cont = false;

uint16_t ModRTU_CRC(uint8_t buf[], int len)
{
    uint16_t crc = 0xFFFF;

    for (int pos = 0; pos < len; pos++)
    {               
        crc ^= (uint16_t)buf[pos];          // XOR byte into least sig. byte of crc

        for (int i = 8; i != 0; i--)
        {
            if ((crc & 0x0001) != 0)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
                crc >>= 1;
        }
    }
    return crc;
}


Context *context;

void setup()
{
	///////// for serial monitor /////////
	Serial.begin(115200);
	// initialize internal parameters
	////////////// for led ///////////////

	context = new Context(new initialization_state);
}

void loop()
{
	// put your main code here, to run repeatedly:

	// delay(100);

	context->do_work();

	uint8_t buffer[100];		// stores the return buffer on each loop
	if (Serial.available() > 0) // bytes received
	{
		Serial.readBytes(msg, MSG_LEN); // binary messages have fixed length and no terminating \0.
		// check the CRC
		// if CRC does not match, discard the message
		crc_received = (msg[MSG_LEN - 2] << 8) | msg[MSG_LEN - 1];
		crc_calculated = ModRTU_CRC(msg, MSG_LEN - 2);
		// crc_received does not match crc_calculated send no response
		if (crc_received != crc_calculated)
		{
		return;
		}
		server_address = msg[0];
		function_code = msg[1];

		if (server_address == 1)
		{
			switch (function_code)
			{
			case 6: // WRITE
				// turn two bytes into a 16-bit integer
				register_address = (msg[2] << 8) | msg[3];
				// if illegal register address, send exception response
				if (register_address > REG_LEN)
				{
					buffer[0] = server_address;
					buffer[1] = function_code + 0x80;
					buffer[2] = 0x02;
					crc_send = ModRTU_CRC(buffer, 3);
					buffer[3] = (crc_send >> 8) & 0xFF;
					buffer[4] = crc_send & 0xFF;
					Serial.write(buffer, 5);
					return;
				}
				
				register_value = (msg[4] << 8) | msg[5];
				reg[register_address] = register_value;

				if ((command == 0x1) && (last_command != 0x1))
					{
					context->command_set_operational();
					last_command = command;
					}

				if ((command == 0x80) && (last_command != 0x80))
					{
					context->command_set_preoperational();
					last_command = command;
					}

				if ((command == 0x81) && (last_command != 0x81))
					{
					context->reset();
					last_command = command;
					}

				if ((command == 0x2) && (last_command != 0x2))
					{
					context->command_stop();
					last_command = command;
					}

				buffer[0] = server_address;
				buffer[1] = function_code;
				buffer[2] = msg[2];
				buffer[3] = msg[3];
				buffer[4] = msg[4];
				buffer[5] = msg[5];

				// calculate the CRC
				crc_send = ModRTU_CRC(buffer, MSG_LEN-2);
				buffer[MSG_LEN-2] = (crc_send >> 8) & 0xFF;
				buffer[MSG_LEN-1] = crc_send & 0xFF;

				Serial.write(buffer, MSG_LEN);
				break;
			case 3: // READ
				// register address of the first register to read
				// turn two bytes into a 16-bit integer
				//speed_reg = speed + 140;
				register_address = (msg[2] << 8) | msg[3];
				total_to_read = (msg[4] << 8) | msg[5];

				// if illegal register address, send exception response
				if (register_address + total_to_read > REG_LEN)
				{
					buffer[0] = server_address;
					buffer[1] = function_code + 0x80;
					buffer[2] = 0x02;
					crc_send = ModRTU_CRC(buffer, 3);
					buffer[3] = (crc_send >> 8) & 0xFF;
					buffer[4] = crc_send & 0xFF;
					Serial.write(buffer, 5);
					return;
				}

				buffer[0] = server_address;
				buffer[1] = function_code;
				buffer[2] = total_to_read * 2;

				for (uint8_t i = 0; i < total_to_read; i++)
				{
					// split the 16-bit integer into two bytes
					buffer[3 + i * 2] = (reg[register_address + i] >> 8) & 0xFF;
					buffer[4 + i * 2] = reg[register_address + i] & 0xFF;
				}
				// calculate the CRC
				crc_send = ModRTU_CRC(buffer, 3 + total_to_read * 2);
				buffer[3 + total_to_read * 2] = (crc_send >> 8) & 0xFF;
				buffer[4 + total_to_read * 2] = crc_send & 0xFF;
				// send the buffer back to the client
				Serial.write(buffer, 3 + total_to_read * 2);
			default:
			          // illegal function code
				buffer[0] = server_address;
				buffer[1] = function_code | 0x80;
				buffer[2] = 0x01; //exception code 0x01
				// calculate the CRC
				crc_send = ModRTU_CRC(buffer, 3);
				buffer[3] = (crc_send >> 8) & 0xFF;
				buffer[4] = crc_send & 0xFF;
				// send the buffer back to the client
				Serial.write(buffer, 5);
				break;
			}
		}
	}
}

// interrupts at every every pulse
ISR(INT0_vect)
{
	// increment/decrement counter (counting pulses)
	enc.position(encoder_input1.is_hi(), encoder_input2.is_hi());
}

// interrupts at update rate
ISR(TIMER0_COMPA_vect)
{
	update_time += 1;
}

// interrupts at every interval ms and calculates speed
// then it resets the pulse counter
ISR(TIMER1_COMPA_vect)
{

	// counter used to print at equal time intervals
	timer1_int_count += constants::interval;
	timer1_int_count_2 += constants::interval;

	// calculate the speed
	speed = (((enc.get_counter() / constants::interval) * 1000.0) / 1400.0) * 60.0;

	enc.reset_counter();

	// kveikja/slokkva led x sinni a sekundu (x Hz)
	if (((timer1_int_count % (1000 / led_freq)) == 0) && (cont == false))
	{
		led.toggle();
		timer1_int_count = 0;
	}

	// for plots
	/* if ((start == true) && (index < 200))
	{
		speed_array[index] = speed;
		//duty_cycle_array[index] = duty_cycle;
		index += 1;
	} */
}

///////// PWM interrupts //////////
ISR(TIMER2_COMPA_vect)
{
	PWM_pin.set_hi();
}

ISR(TIMER2_COMPB_vect)
{
	PWM_pin.set_lo();
}

///////////////////////////////////

// The other external interrupt pin (not used)
// ISR (INT1_vect)
//{
//	/* interrupt handler code here */
//	Serial.print('int1 active');
//	Serial.print('\n');
//}