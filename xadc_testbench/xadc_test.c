#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <linux/types.h>
#define datalength 200


#define Temperature 0x00
#define Vccint 0x01
#define Configuration0 0x40
#define Configuration1 0x41
#define Configuration2 0x42
#define Sequencer 0x48

struct reg{
	unsigned int addres;
	unsigned int Value;
};

void writereg(int fd,struct reg cmd){
	unsigned int buff[2];
	buff[0]=cmd.addres;
	buff[1]=cmd.Value;
	write(fd,buff,sizeof(unsigned int)*2);
}

unsigned int readreg(int fd,struct reg cmd){
	unsigned int buff[2];
	buff[0]=cmd.addres;
	buff[1]=cmd.Value;
	read(fd, buff, sizeof(unsigned int)*2);
	return buff[1];
}

int main()
{
	int *data,*buff;
	data= (int *) malloc(sizeof(int) * datalength);
    buff= (int *) malloc(sizeof(int) * datalength);

   int fd;
   fd = open("/proc/xadc_driver", O_RDWR);
   printf("%d\n",fd);

   struct reg regcmd={
  		   .addres=0x210,
  		   .Value=0x00000000,
     };

   printf("Address:%08lx , reg=%08lx\n",regcmd.addres, readreg(fd,regcmd));
   int fd2;
      fd2 = open("/dev/axidma", O_RDWR);
      read(fd2, buff, sizeof(int)*datalength);
   close(fd);
   close(fd2);

}
