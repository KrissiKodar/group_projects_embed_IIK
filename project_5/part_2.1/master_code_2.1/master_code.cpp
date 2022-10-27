#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<stdint.h>
#include<string.h>


// part 2

int main(int argc, char *argv[]){
   int file, count;
   if(argc!=2){
       printf("Invalid number of arguments, exiting!\n");
       return -2;
   }
   if ((file = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY))<0){
      perror("UART: Failed to open the file.\n");
      return -1;
   }
   struct termios options;
   tcgetattr(file, &options);
   options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
   options.c_iflag = IGNPAR | ICRNL;
   tcflush(file, TCIFLUSH);
   tcsetattr(file, TCSANOW, &options);
   
   char transmit[100];
   
   while (true)   
   {      // send the string plus the null character
      printf("transmit string: ");
      fgets(transmit, 100, stdin);
      if ((count = write(file, &transmit, 100))<0){
         perror("Failed to write to the output\n");
         return -1;
      }
      usleep(100000);
      unsigned char receive[100];
      if ((count = read(file, (void*)receive, 100))<0){
         perror("Failed to read from the input\n");
         return -1;
      }
      if (count==0) printf("There was no data available to read!\n");
      else {
         receive[count]=0;  //There is no null character sent by the Arduino
         printf("[%d]: %s\n",count,receive);
      }
   }

   close(file);
   return 0;
}