#include <Arduino.h>

const size_t MSG_LEN = 8;
uint8_t msg[MSG_LEN];

uint16_t reg[2] = {64, 511};
const size_t REG_LEN = sizeof(reg) /sizeof(reg[0]);

uint8_t server_address;
uint8_t function_code;
uint16_t register_address;
uint16_t register_value;
uint16_t total_to_read;

uint16_t crc_received; 
uint16_t crc_calculated;
uint16_t crc_send;

const unsigned char CMD_LED = 0x02;

int ledPin = 11; // LED with PWM brightness control

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
          
          // write to register
          reg[register_address] = register_value;
          
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
          Serial.write(buffer, 5 + total_to_read * 2);
          break;
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