#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>

int main()
{
    //Enable gpio19
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, "19", 2);
    close(fd);

    //Set gpio19 as input
    fd = open("/sys/class/gpio/gpio19/direction", O_WRONLY);
    write(fd, "in", 2);
    close(fd);

    //Set gpio19 interrupt
    fd = open("/sys/class/gpio/gpio19/edge", O_WRONLY);
    //write(fd, "falling", 7);
    write(fd, "both", 4);
    close(fd);


    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLPRI;

    int counter = 0;
    int last_check = 0;
    while(1)
    {
        //Wait for event
        fd = open("/sys/class/gpio/gpio19/value", O_RDONLY);       
        int ret = poll(&pfd, 1, 3000);
        char c;
        read(fd, &c, 1);
        close(fd);
        if(ret == 0)
            printf("Timeout\n");
        else
            if(c == '0')
                {//printf("Push\n");
                last_check = 0;
                }
            else
                {
                if (last_check == 0)
                    {
                    printf("Counter: %d \n", counter);
                    counter++;
                    last_check = 1;
                    }
                }
    }

    //Disable gpio19
    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    write(fd, "19", 2);
    close(fd);

    return(0);
}