PWD = $(shell pwd)  
KERNEL_SRC = /home/shilei/zybo-linux/Linux-Digilent-Dev-master 
   
obj-m := axi_dma_test.o  
module-objs := axi_dma_test.o  
   
all:  
	       $(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules  
  
         
clean:  
	       rm *.ko  
	       rm *.o  
       
