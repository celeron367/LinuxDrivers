PWD = $(shell pwd)  
KERNEL_SRC = /lib/modules/4.8.0-36-generic/build 
   
obj-m := testdriver.o  
module-objs := testdriver.o  
   
all:  
	       $(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules  
  
         
clean:  
	       rm *.ko  
	       rm *.o  
       
