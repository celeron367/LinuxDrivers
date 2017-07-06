#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#define datalength 1024*7

int main()
{
   int *data,*buff;
   data= (int *) malloc(sizeof(int) * datalength);
   buff= (int *) malloc(sizeof(int) * datalength);

   int i=0;
   for(i;i<datalength;i++){
	   data[i]=i;
   }
   int fd;
   fd = open("/dev/axidma", O_RDWR);
   printf("%d\n",fd);
   write(fd, data, sizeof(int)*datalength);
   read(fd, buff, sizeof(int)*datalength);
   close(fd);
   i=0;
   for(i;i<datalength;i++){
	   printf("%d,%d\n",data[i],buff[i]);
   }

}
