#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>

int main()
{
    //Enable gpio22
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, "22", 2);
    close(fd);
    //Enable gpio17
    int op = open("/sys/class/gpio/export", O_WRONLY);
    write(op, "17", 2);
    close(op);

    //Set gpio22 as input
    fd = open("/sys/class/gpio/gpio22/direction", O_WRONLY);
    write(fd, "in", 2);
    close(fd);
    //Set gpio17 as output
    op = open("/sys/class/gpio/gpio17/direction", O_WRONLY);
    write(op, "out", 2);
    close(op);

    //Set gpio25 interrupt
    fd = open("/sys/class/gpio/gpio22/edge", O_WRONLY);
    //write(fd, "falling", 7);
    write(fd, "both", 4);
    close(fd);

    // struct pollfd pfd;
    // pfd.fd = fd;
    // pfd.events = POLLPRI;

    for(int i=0; i<10000000; i++)
    {
        //Wait for event
        fd = open("/sys/class/gpio/gpio22/value", O_RDONLY);       
        op = open("/sys/class/gpio/gpio17/value", O_WRONLY);       
        // int ret = poll(&pfd, 1, 3000);
        char c;
        read(fd, &c, 1);
        close(fd);
        // if(ret == 0)
        //     printf("Timeout\n");
        // else
        if(c == '0'){
            printf("Push\n");
            write(op, "00", 2);}
        else{
            printf("Release\n");
            write(op, "01", 2);}
        close(op);
    }

    //Disable gpio22
    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    write(fd, "22", 2);
    close(fd);
    //Disable gpio17
    op = open("/sys/class/gpio/unexport", O_WRONLY);
    write(op, "17", 2);
    close(op);

    return(0);
}