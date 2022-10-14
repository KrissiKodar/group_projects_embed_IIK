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
#include<iostream>
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


void set_pwm(char* duty_cycle_to_PWM)
{
    int fd;
    fd = open("/sys/class/pwm/pwmchip0/pwm1/duty_cycle", O_WRONLY);
    write(fd, duty_cycle_to_PWM, sizeof(int));
    close(fd);
}

static char receive[sizeof(int)];     ///< The receive buffer from the LKM
static char duty_cycle_to_PWM[sizeof(int)] = {0};

int main(){
    int ret, fd;
    int encoder_count;
    int msec;

    int exit_counter = 0;
    int end_program_check = 30;

    float reference_speed;
    float speed = 0;
    float average_speed = 0;
    const int period = 6000;
    float duty_cycle = 0;

    // Print output
    int print_time_counter = 0;
    int print_every_x_msec = 500;

    const float measurement_interval = 20; //msec


    // timer (using relative timers not delay)
    struct timespec start, end;

    // PI controller
    PI_Controller controller;
    controller.init(1.8/140.0, 0.048, measurement_interval/1000.0, 1.0);
    //Enable gpio22
    fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, "22", 2);
    close(fd);

    //Set gpio22 as output
    fd = open("/sys/class/gpio/gpio22/direction", O_WRONLY);
    write(fd, "out", 3);
    close(fd);

    //Set gpio22 as high
    fd = open("/sys/class/gpio/gpio22/value", O_WRONLY);
    write(fd, "1", 1);
    close(fd);

    //Enable gpio27
    fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, "27", 2);
    close(fd);

    //Set gpio27 as output
    fd = open("/sys/class/gpio/gpio27/direction", O_WRONLY);
    write(fd, "out", 3);
    close(fd);

    // set gpio27 low
    fd = open("/sys/class/gpio/gpio27/value", O_WRONLY);
    write(fd, "0", 1);
    close(fd);

    // next we make gpio13 a PWM output
    fd = open("/sys/class/pwm/pwmchip0/export", O_WRONLY);
    write(fd, "1", 1);
    close(fd);

    // set the period of pwm
    fd = open("/sys/class/pwm/pwmchip0/pwm1/period", O_WRONLY);
    write(fd, "6000", 4);
    close(fd);

    // set the duty cycle to 0ms
    fd = open("/sys/class/pwm/pwmchip0/pwm1/duty_cycle", O_WRONLY);
    write(fd, "0", 1);
    close(fd);

    // enable the PWM
    fd = open("/sys/class/pwm/pwmchip0/pwm1/enable", O_WRONLY);
    write(fd, "1", 1);
    close(fd);

    // ask user input for reference speed
    printf("PROGRAM START\n");
    std::cout << "Enter reference speed: ";
    std::cin >> reference_speed;
    printf("The reference speed is %d \n", reference_speed);

    printf("Starting device test code example...\n");
    fd = open("/dev/motor_encoder", O_RDWR);             // Open the device with read/write access
    if (fd < 0){
        perror("Failed to open the device...");
        return errno;
    }
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    while(1)
    {
        clock_gettime(CLOCK_MONOTONIC, &end);
        msec = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
        if (msec > measurement_interval)
        {
            // Read the response from the LKM
            // i.e. read the encoder counter from the LKM
            // every time it is read the counter variable is reset to 0 inside the LKM
            // for the speed measurement
            ret = read(fd, receive, sizeof(int));
            if (ret < 0)
            {
                perror("Failed to read the message from the device.");
                return errno;
            }
            
            encoder_count = strtol(receive, nullptr, 10);
            speed = (((encoder_count/measurement_interval) * 1000.0) / 700.0) * 60.0;
            average_speed = average_speed + (speed - average_speed) / 10.0;
            // print speeds at update rate (hard to read!!!)
            // clear terminal after every print
            //printf("Actual speed: %.2f \n", speed);
            //printf("Average speed: %.2f \n", average_speed);
            //printf("The duty cycle is %f \n", duty_cycle);
            duty_cycle = controller.update(reference_speed, speed);
            // CW
            if (duty_cycle < 0)
            {
                duty_cycle = 0;
            }

            int set_duty_cycle = duty_cycle * period;
            sprintf(duty_cycle_to_PWM, "%i", set_duty_cycle);
            set_pwm(duty_cycle_to_PWM);
           
            print_time_counter += measurement_interval;
            clock_gettime(CLOCK_MONOTONIC, &start);
        }


        if(print_time_counter > print_every_x_msec)
        {
            // clear terminal after every print
            printf("\033[2J\033[1;1H");
            printf("Actual speed: %.2f \n", speed);
            printf("Average speed: %.2f \n", average_speed);
            printf("The duty cycle is %f \n", duty_cycle);
            exit_counter++;
            print_time_counter = 0;
        }

        // When the exit counter is larger than the end program check, the program will exit (exit condition)
        if(exit_counter > end_program_check)
        {
            ret = close(fd);
            // disable gpio22, gpio27 and pwm0
            fd = open("/sys/class/gpio/unexport", O_WRONLY);
            write(fd, "22", 2);
            close(fd);
            fd = open("/sys/class/gpio/unexport", O_WRONLY);
            write(fd, "27", 2);
            close(fd);
            fd = open("/sys/class/pwm/pwmchip0/unexport", O_WRONLY);
            write(fd, "1", 1);
            close(fd);

            break;
        }
    }
    
    printf("End of the program\n");
    return 0;
}