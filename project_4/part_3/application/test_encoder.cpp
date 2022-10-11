/**
 * @file   testebbchar.c
 * @author Derek Molloy
 * @date   7 April 2015
 * @version 0.1
 * @brief  A Linux user space program that communicates with the ebbchar.c LKM. It passes a
 * string to the LKM and reads the response from the LKM. For this example to work the device
 * must be called /dev/ebbchar.
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
*/
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
 
//#define BUFFER_LENGTH 4             ///< The buffer length (crude but fine)

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

static char receive[sizeof(int)];     ///< The receive buffer from the LKM
static char duty_cycle_to_PWM[sizeof(int)] = {0};

int main(){
    int ret, fd;
    int msec;
    int exit_counter = 0;
    int end_program_check = 20;

    const int reference_speed = 100;
    int speed = 0;
    float duty_cycle = 0;

    //int update_time;
    int measurement_time;
    int print_time;
    //int update_interval = 6;
    const int measurement_interval = 20;

    // timer (using relative timers not delay)
    struct timespec start, end;

    // PI controller
    PI_Controller controller;
    controller.init(1.8/140.0, 0.048, 0.006, 1.0);
    //Enable gpio22
    fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, "22", 2);
    close(fd);

    //Set gpio22 as output
    fd = open("/sys/class/gpio/gpio22/direction", O_WRONLY);
    write(fd, "out", 2);
    close(fd);

    //Enable gpio27
    fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, "27", 2);
    close(fd);

    //Set gpio27 as output
    fd = open("/sys/class/gpio/gpio27/direction", O_WRONLY);
    write(fd, "out", 2);
    close(fd);

    // next we make gpio13 a PWM output
    fd = open("/sys/class/pwm/pwmchip0/export", O_WRONLY);
    write(fd, "0", 1);
    close(fd);

    // set the period to 15ms
    fd = open("/sys/class/pwm/pwmchip0/pwm0/period", O_WRONLY);
    write(fd, "15000000", 8);
    close(fd);

    // set the duty cycle to 0ms
    fd = open("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", O_WRONLY);
    write(fd, "0", 7);
    close(fd);

    // enable the PWM
    fd = open("/sys/class/pwm/pwmchip0/pwm0/enable", O_WRONLY);
    write(fd, "1", 1);
    close(fd);

    printf("Starting device test code example...\n");
    fd = open("/dev/motor_encoder", O_RDWR);             // Open the device with read/write access
    if (fd < 0){
        perror("Failed to open the device...");
        return errno;
    }
    printf("The reference speed is %d", reference_speed);
    clock_gettime(CLOCK_MONOTONIC, &start);
    while(1)
    {
        clock_gettime(CLOCK_MONOTONIC, &end);
        msec = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
        if (msec > 1)
        {
            // update timer variables
            //update_time++;
            measurement_time++;
            print_time++;
            clock_gettime(CLOCK_MONOTONIC, &start);
        }

        if (measurement_time > measurement_interval)
        {
            // read encoder
            ret = read(fd, receive, sizeof(int));        // Read the response from the LKM
            if (ret < 0)
            {
                perror("Failed to read the message from the device.");
                return errno;
            }
            int encoder_count = *((int*)receive);
            //int encoder_count = strtol(receive, nullptr, 10);
            speed = (((encoder_count / measurement_interval) * 1000.0) / 700.0) * 60.0;

            duty_cycle = controller.update(reference_speed, speed);
            // CW
            if (duty_cycle < 0)
            {
                duty_cycle = 0;
            }
            // set the duty cycle
            int set_duty_cycle = duty_cycle * 15000000;
            sprintf(duty_cycle_to_PWM, "%d", set_duty_cycle);
            fd = open("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", O_WRONLY);
            write(fd, duty_cycle_to_PWM, sizeof(int));
            close(fd);
            measurement_time = 0;
        }

        if(print_time > 1000)
        {
            ret = read(fd, receive, sizeof(int));        // Read the response from the LKM
            if (ret < 0)
            {
                perror("Failed to read the message from the device.");
                return errno;
            }
            /* printf("The received message is: [%s]\n", receive);
            receive_int = strtol(receive, nullptr, 10);
            printf("The counter value is: [%i]\n", receive_int); */
            printf("The actual speed is: [%i]\n", speed);
            exit_counter++;
            print_time = 0;
        }

        // after en_program_time seconds exit and close the device
        if(exit_counter > end_program_check)
        {
            ret = close(fd);
            break;
        }
    }
    
    printf("End of the program\n");
    return 0;
}