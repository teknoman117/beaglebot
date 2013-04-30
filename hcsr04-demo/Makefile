CC=gcc
PRU_ASM=pasm

all:
	@echo ">> Generating PRU binary"
	$(PRU_ASM) -b hcsr04_demo.p
	@echo ">> Compiling Sonar Example"
	$(CC) -c -o hcsr04_demo.o hcsr04_demo.c
	$(CC) -lpthread -lprussdrv -o hcsr04_demo hcsr04_demo.o
	
clean:
	rm -rf hcsr04_demo hcsr04_demo.o hcsr04_demo.bin 
    
