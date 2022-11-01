#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdint.h> // uint8_t
#include <cstdlib>  // atoi()
#include <pthread.h>

int main(int argc, char *argv[])
{
    int file, count;

    uint16_t reg[2] = {0x0000, 0x0000};

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

    const size_t MSG_LEN = 6;
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

    // send the fixed length message
    if ((count = write(file, msg, MSG_LEN)) < 0)
    {
        perror("Failed to write to the output\n");
        return -1;
    }
    printf("Sent request: %02X %02X %02X %02X %02X %02X\n",msg[0],msg[1],msg[2],msg[3],msg[4],msg[5]);


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
    else
    {
        printf("Received reply: ");
        for (int i = 0; i < count; i++)
        {
            printf("%02X ", receive[i]);
        }
        printf("\n");
    }

    close(file);

    return 0;
}