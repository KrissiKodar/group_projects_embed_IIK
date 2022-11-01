#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdint.h> // uint8_t
#include <cstdlib>	// atoi()
#include <pthread.h>

class PI_Controller: public Controller
{
public:
    void init(float Kp, float Ti, float integration_T, float max_output)
	{
		this->Kp = Kp;
		this->Ti = Ti;
		this->integration_T = integration_T;
		this->max_output = max_output;
		sum_error = 0;
	}
    float update(float ref, float actual)
	{
		error = ref - actual;
		sum_error += error * integration_T;
		output =  Kp * (error + sum_error/Ti);
		// if output is saturated, do not integrate
		// output is saturated if it is smaller than -1 or larger than 1
		if (output >= max_output)
		{
			output = max_output;
			sum_error -= error * integration_T;
		}
		else if (output <= 0)//-max_output)
		{
			output = 0;
			//output = -max_output;
			sum_error -= error * integration_T;
		}
		return output;
	}
private:
    float error;
    float Kp;
    float Ti;
    float integration_T;
    float max_output = 1.0;
    float sum_error = 0;
    float output = 0;
};

int main(int argc, char *argv[])
{
	int file, count;

	int reference_speed = 0;
	int reference_brightness = 100;
	int brightness = 0;
	PI_Controller controller;
	controller.init(0.1, 0.1, 0.01, 140.0);
	//uint16_t reg[2] = {0x0000, 0x0000};

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


	// two arduinos controlled by this program
	// get measurement from one and value to set on the other
	// modbus message format

	msg[0] = 1;
	msg[1] = 6;

	uint16_t register_address = 0;
	msg[2] = (register_address >> 8) & 0xFF;
	msg[3] = register_address & 0xFF;

	uint16_t register_value = 1;
	msg[4] = (register_value >> 8) & 0xFF;
	msg[5] = register_value & 0xFF;

	// send the fixed length message
	if ((count = write(file, msg, MSG_LEN)) < 0)
	{
		perror("Failed to write to the output\n");
		return -1;
	}

	usleep(100000);

	uint8_t receive[100];
	// read the value from the first arduino
	if ((count = read(file, (void *)receive, 100)) < 0)
	{
		perror("Failed to read from the input\n");
		return -1;
	}
	else if (count == 0)
	{
		printf("There was no data available to read!\n");
	}

	while(true)
	{	
		msg[0] = 2;
		msg[1] = 3;

		register_address = 1;
		msg[2] = (register_address >> 8) & 0xFF;
		msg[3] = register_address & 0xFF;

		register_value = 1;
		msg[4] = (register_value >> 8) & 0xFF;
		msg[5] = register_value & 0xFF;

		// send the fixed length message
		if ((count = write(file, msg, MSG_LEN)) < 0)
		{
			perror("Failed to write to the output\n");
			return -1;
		}

		usleep(100000);

		uint8_t receive[100];
		// read the value from the first arduino
		if ((count = read(file, (void *)receive, 100)) < 0)
		{
			perror("Failed to read from the input\n");
			return -1;
		}
		else if (count == 0)
		{
			printf("There was no data available to read!\n");
		}

		brightness = (receive[3] << 8) | receive[4];
		printf("Brightness: %d\n", brightness);

		reference_speed = controller.update(reference_brightness, brightness);

		msg[0] = 1;
		msg[1] = 6;

		register_address = 2;
		msg[2] = (register_address >> 8) & 0xFF;
		msg[3] = register_address & 0xFF;

		register_value = reference_speed;
		msg[4] = (register_value >> 8) & 0xFF;
		msg[5] = register_value & 0xFF;

		if ((count = write(file, msg, MSG_LEN)) < 0)
		{
			perror("Failed to write to the output\n");
			return -1;
		}


	}

	close(file);

	return 0;
}