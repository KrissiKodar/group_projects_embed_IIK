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
#include<iostream>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
 
//#define BUFFER_LENGTH 4             ///< The buffer length (crude but fine)

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
    int msec;
    int exit_counter = 0;
    int end_program_check = 5;
    int settt_pwm = 0;
    float duty_cycle;
    const int period = 6000;
    int print_time = 0;


    // timer (using relative timers not delay)
    struct timespec start, end;
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

    // set the period 
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

    // ask user input for duty cycle
    std::cout << "Enter duty cycle: ";
    std::cin >> duty_cycle;
    std::cout << "You entered: " << duty_cycle << std::endl;


    clock_gettime(CLOCK_MONOTONIC, &start);
    while(1)
    {
        clock_gettime(CLOCK_MONOTONIC, &end);
        msec = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
        if (msec > 1000)
        {
            //duty_cycle = duty_cycle + 0.1;
            int set_duty_cycle = duty_cycle * period;
            set_duty_cycle = (set_duty_cycle)%period;
            printf("set_duty_cycle: %i \n", set_duty_cycle);
            sprintf(duty_cycle_to_PWM, "%i", set_duty_cycle);
            //printf("duty_cycle_to_PWM: %s \n", duty_cycle_to_PWM);
            set_pwm(duty_cycle_to_PWM);
            exit_counter++;
            clock_gettime(CLOCK_MONOTONIC, &start);
        }

        // after en_program_time seconds exit and close the device
        if(exit_counter > end_program_check)
        {
            fd = open("/sys/class/pwm/pwmchip0/unexport", O_WRONLY);
            write(fd, "1", 1);
            close(fd);

            break;
        }
    }
    
    printf("End of the program\n");
    return 0;
}