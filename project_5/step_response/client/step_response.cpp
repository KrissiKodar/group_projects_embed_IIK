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
		output =  Kp * (error + sum_error/Ti);
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


// int round(int n)
// {
//     // Smaller multiple
//     int a = (n / 10) * 10;
     
//     // Larger multiple
//     int b = a + 10;
 
//     // Return of closest of two
//     return (n - a > b - n)? b : a;
// }

int main(int argc, char *argv[])
{
	int file, count;

	uint16_t reference_speed = 0;
	uint16_t reference_brightness = 700;
	uint16_t brightness = 0;
	int control_signal = 0;
	uint16_t mean_brightness = 600;
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


    // array for storing mesured speed
    uint16_t brightness_array[1000];

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
	
    
	while(true)
	{	
		msg_brightness[0] = 2;
		msg_brightness[1] = 3;

		register_address = 1;
		msg_brightness[2] = (register_address >> 8) & 0xFF;
		msg_brightness[3] = register_address & 0xFF;

		register_value = 1;
		msg_brightness[4] = (register_value >> 8) & 0xFF;
		msg_brightness[5] = register_value & 0xFF;

		// send the fixed length message
		if ((count = write(file, msg_brightness, MSG_LEN)) < 0)
		{
			perror("Failed to write to the output\n");
			return -1;
		}

		usleep(100000);

		// uint8_t receive_brightness[100];
		// read the value from the first arduino
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
		mean_brightness = mean_brightness + (brightness-mean_brightness)/20;  

		

		control_signal = controller.update(reference_brightness, mean_brightness);
		// prent = round(prent); //til að minnka flokt
		// reference_speed = (prent * 2.0 / 7.0) + 100 + 140;
		reference_speed = (control_signal * 0.482) + 72.5 + 140;

		
/* 		if  ((k%10) == 0)
		{
			printf("prent: %d\n", control_signal);
			printf("ref speed %d\n", reference_speed-140);
			printf("Brightness: %d\n", mean_brightness);
			printf("k: %d  \n", k);
			k=0;
		}
 */
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

        brightness_array[k] = brightness;
		k++;
        if (k == 1000)
        {
            break;
        }
	}

    printf("[");
    for (int i = 0; i < 1000; i++)
    {
        printf("%d, ", brightness_array[i]);
    }
    printf("]");

	close(file);

	return 0;
}