PWD = $(shell pwd)  
KERNEL_SRC = /home/shilei/zybo-linux/Linux-Digilent-Dev-master 
   
obj-m := xadc.o  
module-objs := xadc.o  
   
all:  
	       $(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules  
  
         
clean:  
	       rm *.ko  
	       rm *.o  
       
