obj-m = tieqep.o
KDIR := /home/nathaniel/Programming/linux-dev/KERNEL
ccflags-y = -I/home/nathaniel/Programming/linux-dev/KERNEL

all:
	make -C $(KDIR) M=$(shell pwd) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules
	
clean:
	make -C $(KDIR) M=$(shell pwd) clean
