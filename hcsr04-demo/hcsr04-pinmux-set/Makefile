# Object file for the kernel module
obj-m = hcsr04_pinmux_set.o

# IMPORTANT! The directory that the source of your Beaglebone's kernel is located in (yours most likely isn't mine)
#KDIR := /home/nathaniel/Programming/beaglebot-kernel/KERNEL

# Compiler to use (mine was built using crosstools-ng for the arm cortex a8 hardfloat (armv7hf))
#COMPILER_PREFIX := arm-cortex_a8-linux-gnueabi-

# Build the kernel module
all:
	make -C $(KDIR) M=$(shell pwd) ARCH=arm CROSS_COMPILE=$(COMPILER_PREFIX) modules

# Cleanup a kernel module build
clean:
	make -C $(KDIR) M=$(shell pwd) clean
