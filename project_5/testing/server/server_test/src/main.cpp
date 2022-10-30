#include <Arduino.h>

const size_t MSG_LEN = 6;
uint8_t msg[MSG_LEN];

uint16_t reg[2] = {0x0003, 0x0004};

uint8_t server_address;
uint8_t function_code;
uint16_t register_address;
uint16_t register_value;
uint16_t total_to_read;

const unsigned char CMD_LED = 0x02;

int ledPin = 11; // LED with PWM brightness control

void setup()
{
  // A baud rate of 115200 (8-bit with No parity and 1 stop bit)
  Serial.begin(115200, SERIAL_8N1);
  pinMode(ledPin, OUTPUT); // the LED is an output
}

void loop()
{
  uint8_t buffer[100];        // stores the return buffer on each loop
  if (Serial.available() > 0) // bytes received
  {
    Serial.readBytes(msg, MSG_LEN); // binary messages have fixed length and no terminating \0.
    server_address = msg[0];
    function_code = msg[1];
    
    if (server_address == 1)
    {
      switch (function_code)
      {
        case 6: // WRITE
          register_address = (msg[2] << 8) | msg[3];
          register_value = (msg[4] << 8) | msg[5];
          // change value of register at register_address to register_value
          reg[register_address] = register_value;
          memcpy(buffer, msg, MSG_LEN);
          Serial.write(buffer, MSG_LEN);
          break;
        case 3: // READ
          // register address of the first register to read
          register_address = (msg[2] << 8) | msg[3];
          // total number of registers to read
          total_to_read = (msg[4] << 8) | msg[5];
          buffer[0] = server_address;
          buffer[1] = function_code;
          buffer[2] = total_to_read * 2;

          for (uint8_t i = 0; i < total_to_read; i++)
          {
            buffer[3 + i * 2] = (reg[register_address + i] >> 8) & 0xFF;
            buffer[4 + i * 2] = reg[register_address + i] & 0xFF;
          }
          // send the buffer back to the client
          Serial.write(buffer, total_to_read * 2);
        default:
          buffer[0] = 112;
          Serial.write(buffer, 1);
          break;
      } 
    }
  }
}