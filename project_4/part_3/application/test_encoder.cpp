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

static char receive[sizeof(int)];     ///< The receive buffer from the LKM
 
int main(){
    int ret, fd;
    int msec;
    int exit_counter = 0;
    int end_program_time = 20;
    int receive_int;
    // timer (using relative timers not delay)
    struct timespec start, end;

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
        if(msec > 1000)
        {
            ret = read(fd, receive, sizeof(int));        // Read the response from the LKM
            if (ret < 0)
            {
                perror("Failed to read the message from the device.");
                return errno;
            }
            receive_int = atoi(receive);
            printf("The counter value is: [%s]\n", receive_int);
            exit_counter++;
            clock_gettime(CLOCK_MONOTONIC, &start);
        }
        // after en_program_time seconds exit and close the device
        if(exit_counter > end_program_time)
        {
            ret = close(fd);
            break;
        }
    }
    
    printf("End of the program\n");
    return 0;
}