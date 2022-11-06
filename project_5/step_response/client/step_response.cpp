#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdint.h> // uint8_t
#include <cstdlib>	// atoi()
#include <pthread.h>

class PI_Controller
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
		output = Kp * (error + sum_error / Ti);
		// if output is saturated, do not integrate
		// output is saturated if it is smaller than -1 or larger than 1
		if (output >= max_output)
		{
			output = max_output;
			sum_error -= error * integration_T;
		}
		else if (output <= -max_output)
		{
			output = -max_output;
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

	uint16_t reference_speed = 0;
	uint16_t reference_brightness = 400;
	uint16_t brightness = 0;
	int control_signal = 0;
	uint16_t mean_brightness = 300;
	PI_Controller controller;
	controller.init(0.5, 0.1, 0.001, 140.0);

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
	uint8_t msg_brightness[MSG_LEN];
	uint8_t msg_motor[MSG_LEN];

	// two arduinos controlled by this program
	// get measurement from one and value to set on the other
	// modbus message format

	msg_motor[0] = 1;
	msg_motor[1] = 6;

	uint16_t register_address = 0;
	uint8_t receive_brightness[100];
	uint8_t receive_motor[100];
	msg_motor[2] = (register_address >> 8) & 0xFF;
	msg_motor[3] = register_address & 0xFF;

	uint16_t register_value = 1;
	msg_motor[4] = (register_value >> 8) & 0xFF;
	msg_motor[5] = register_value & 0xFF;

	// send the fixed length message
	if ((count = write(file, msg_motor, MSG_LEN)) < 0)
	{
		perror("Failed to write to the output\n");
		return -1;
	}

	usleep(100000);

	int k = 0;
	int j = 0;
	int num_measurements = 100;
	int every_n_samples = 10;
	uint16_t ref_speed_array[num_measurements];
	uint16_t brightness_array[num_measurements];

	while (true)
	{
		msg_brightness[0] = 2;
		msg_brightness[1] = 3;

		register_address = 1;
		msg_brightness[2] = (register_address >> 8) & 0xFF;
		msg_brightness[3] = register_address & 0xFF;

		register_value = 1;
		msg_brightness[4] = (register_value >> 8) & 0xFF;
		msg_brightness[5] = register_value & 0xFF;

		if (k % every_n_samples == 0)
		{
			printf("ref speed %d\n", reference_speed - 140);
			printf("brightness %d\n", mean_brightness);
			brightness_array[j] = mean_brightness;
			ref_speed_array[j] = reference_speed - 140;
			j++;
		}

		k++;
		if (k >= num_measurements * every_n_samples)
		{
			break;
		}

		// send the fixed length message
		if ((count = write(file, msg_brightness, MSG_LEN)) < 0)
		{
			perror("Failed to write to the output\n");
			return -1;
		}

		usleep(100000);

		// read the value from the arduino 1
		if ((count = read(file, (void *)receive_brightness, 100)) < 0)
		{
			perror("Failed to read from the input\n");
			return -1;
		}
		else if (count == 0)
		{
			printf("There was no data available to read!\n");
		}

		brightness = (receive_brightness[3] << 8) | receive_brightness[4];
		mean_brightness = mean_brightness + (brightness - mean_brightness) / 20;

		control_signal = controller.update(reference_brightness, mean_brightness);

		// control signal is scaled from [-140,140] to [5,140] (then +140)
		// because for the brightness control it doesnt make sense to
		// completely stop the disk or go in reverse
		reference_speed = (control_signal * 0.482) + 72.5 + 140;

		msg_motor[0] = 1;
		msg_motor[1] = 6;

		register_address = 2;
		msg_motor[2] = (register_address >> 8) & 0xFF;
		msg_motor[3] = register_address & 0xFF;

		register_value = reference_speed;
		msg_motor[4] = (register_value >> 8) & 0xFF;
		msg_motor[5] = register_value & 0xFF;

		if ((count = write(file, msg_motor, MSG_LEN)) < 0)
		{
			perror("Failed to write to the output\n");
			return -1;
		}

		usleep(100000);

		if ((count = read(file, (void *)receive_motor, 100)) < 0)
		{
			perror("Failed to read from the input\n");
			return -1;
		}
		else if (count == 0)
		{
			printf("There was no data available to read!\n");
		}
		k++;
	}

	printf("[");
	for (int i = 0; i < num_measurements * every_n_samples; i++)
	{
		printf("%d, ", brightness_array[i]);
	}
	printf("]");
	printf("\n");
	printf("[");
	for (int i = 0; i < num_measurements * every_n_samples; i++)
	{
		printf("%d, ", ref_ref_speed_array[i]);
	}
	printf("]");

	close(file);

	return 0;
}