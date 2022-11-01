#include <Arduino.h>

const size_t MSG_LEN = 6;
uint8_t msg[MSG_LEN];

uint16_t reg[2] = {0, 0}; // commands, brightness, mean_brightness

uint8_t server_address;
uint8_t function_code;
uint16_t register_address;
uint16_t register_value;
uint16_t total_to_read;

const int sensorPin = 0;
uint16_t lightVal = 0;
//uint16_t& lightVal = reg[1];
uint16_t& meanLightVal = reg[1];

void setup()
{
  // A baud rate of 115200 (8-bit with No parity and 1 stop bit)
  Serial.begin(115200, SERIAL_8N1);
}

void loop()
{
  
  lightVal = analogRead(sensorPin);
  meanLightVal = (meanLightVal + lightVal) / 10;
  
  uint8_t buffer[100];        // stores the return buffer on each loop
  if (Serial.available() > 0) // bytes received
  {
    Serial.readBytes(msg, MSG_LEN); // binary messages have fixed length and no terminating \0.
    server_address = msg[0];
    function_code = msg[1];
    
    if (server_address == 2)
    {
      switch (function_code)
      {
        case 6: // WRITE
          // turn two bytes into a 16-bit integer
          register_address = (msg[2] << 8) | msg[3];
          register_value = (msg[4] << 8) | msg[5];
          reg[register_address] = register_value;
          memcpy(buffer, msg, MSG_LEN);
          Serial.write(buffer, MSG_LEN);
          break;
        case 3: // READ
          // register address of the first register to read
          // turn two bytes into a 16-bit integer
          register_address = (msg[2] << 8) | msg[3];
          total_to_read = (msg[4] << 8) | msg[5];
          buffer[0] = server_address;
          buffer[1] = function_code;
          buffer[2] = total_to_read * 2;

          for (uint8_t i = 0; i < total_to_read; i++)
          {
            // split the 16-bit integer into two bytes
            buffer[3 + i * 2] = (reg[register_address + i] >> 8) & 0xFF;
            buffer[4 + i * 2] = reg[register_address + i] & 0xFF;
          }
          // send the buffer back to the client
          Serial.write(buffer, 3 + total_to_read * 2);
        default:
          break;
      } 
    }
  }
}