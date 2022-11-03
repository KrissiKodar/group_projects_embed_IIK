#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdint.h> // uint8_t
#include <cstdlib>  // atoi()
#include <pthread.h>


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


int main(int argc, char *argv[])
{
    int file, count;

    if ((file = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
    {
        perror("UART: Failed to open the file.\n");
        return -1;
    }

    struct termios options;
    tcgetattr(file, &options);
    cfmakeraw(&options); // set serial port to raw mode for binary comms

    options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
    options.c_iflag = IGNPAR | ICRNL;

    tcflush(file, TCIFLUSH);
    tcsetattr(file, TCSANOW, &options);

    const size_t MSG_LEN = 8;
    uint8_t msg[MSG_LEN];

    // populate the message with integer values in binary format
    msg[0] = atoi(argv[1]);
    msg[1] = atoi(argv[2]);

    uint16_t register_address = atoi(argv[3]);
    msg[2] = (register_address >> 8) & 0xFF;
    msg[3] = register_address & 0xFF;

    uint16_t register_value = atoi(argv[4]);
    msg[4] = (register_value >> 8) & 0xFF;
    msg[5] = register_value & 0xFF;

    uint16_t crc = ModRTU_CRC(msg, MSG_LEN - 2);
    msg[6] = (crc >> 8) & 0xFF;
    msg[7] = crc & 0xFF;
    

    // send the fixed length message
    if ((count = write(file, msg, MSG_LEN)) < 0)
    {
        perror("Failed to write to the output\n");
        return -1;
    }
    printf("Sent request: %02x %02x %02x %02x %02x %02x %02x %02x\n",msg[0],msg[1],msg[2],msg[3],msg[4],msg[5],msg[6],msg[7]);


    usleep(100000);

    uint8_t receive[100];
    // read the response into the receive buffer
    if ((count = read(file, (void *)receive, 100)) < 0)
    {
        perror("Failed to read from the input\n");
        return -1;
    }
    else if (count == 0)
    {
        printf("There was no data available to read!\n");
    }

    // check if CRC in received message is correct
    uint16_t crc_received = (receive[count - 2] << 8) | receive[count - 1];
    uint16_t crc_calculated = ModRTU_CRC(receive, count - 2);
    if (crc_received == crc_calculated)
    {
        printf("Received reply: ");
        for (int i = 0; i < count; i++)
        {
            printf("%02x ", receive[i]);
        }
        printf("\n");
    }
    

    close(file);

    return 0;
}