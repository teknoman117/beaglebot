// Define the entry point of the program
.origin 0
.entrypoint START

// Address of the io controller for GPIO1
#define GPIO1              0x4804c000

// Offset address for the output enable register of the gpio controller
#define GPIO_OE            0x134

// Offset address for the data in register of the gpio controller
#define GPIO_DATAIN        0x138

// Offset address for the clear data out register of the gpio controller
#define GPIO_CLEARDATAOUT  0x190

// Offset address for the set data out register of the gpio controller
#define GPIO_SETDATAOUT    0x194

// PRU interrupt for PRU0
#define PRU0_ARM_INTERRUPT 19

// Initialize hardware
START:
    // Allow the PRU to access memories outside its own map (just code I found on the interwebs)
    LBCO r0, C4, 4, 4
    CLR r0, r0, 4
    SBCO r0, C4, 4, 4
    
    // Make constant 24 (c24) point to the beginning of PRU0's data ram
    MOV       r0, 0x00000000
    MOV       r1, 0x22020
    SBBO      r0, r1, 0, 4

    // Enable trigger as output and echo as input (clear bit 6 of output enable and set bit 7)
    MOV r3, GPIO1 | GPIO_OE
    LBBO r2, r3, 0, 4
    CLR r2, 6
    SET r2, 7
    SBBO r2, r3, 0, 4

    // Fire the sonar
TRIGGER:
    // Set trigger pin to high
    MOV r2, 1<<6
    MOV r3, GPIO1 | GPIO_SETDATAOUT
    SBBO r2, r3, 0, 4
    
    // Delay 10 microseconds (200 MHz / 2 instructions = 10 ns per loop, 10 us = 1000 loops) 
    MOV r0, 1000
TRIGGER_DELAY_10US:
    SUB r0, r0, 1
    QBNE TRIGGER_DELAY_10US, r0, 0
    
    // Set trigger pin to low
    MOV r2, 1<<6
    MOV r3, GPIO1 | GPIO_CLEARDATAOUT
    SBBO r2, r3, 0, 4
    
    // Wait for the echo to go low, i.e. wait for the echo cycle to start
    MOV r3, GPIO1 | GPIO_DATAIN
WAIT_ECHO:
    // Read the GPIO in register
    LBBO r2, r3, 0, 4
    
    // Mask all bits but the desired one
    AND r2, r2, 1<<7
    
    // If the register is 0, means the pin went low
    QBNE WAIT_ECHO, r2, 0
     
    // Set r4 to zero, will be used to count the microseconds of the cho
    MOV r4, 0
SAMPLE_ECHO:
    // Read the GPIO in register 
    LBBO r2, r3, 0, 4
    
    // Mask all but the desired bit
    AND r2, r2, 1<<7
    
    // If the pin is not low, (i.e. not zero because of mask), branch to echo complete 
    QBNE ECHO_COMPLETE, r2, 0
    
    // Delay 1 microsecond (delay itself is less than a microseconds because it takes time to query the GPIO
    // register due to it not being within the local address space of the PRU
    MOV r0, 79
SAMPLE_ECHO_DELAY_1US:
    SUB r0, r0, 1
    QBNE SAMPLE_ECHO_DELAY_1US, r0, 0
    
    // Add one to the microsecond counter
    ADD r4, r4, 1
    
    // Jump back to sampling the echo pin
    JMP SAMPLE_ECHO  
     
    // When the echo is complete, do this
ECHO_COMPLETE:
    // Store the microsecond count in the PRU's data ram
    SBCO r4, c24, 0, 4
    
    // Trigger the PRU0 interrupt (C program recognized the event)
    MOV r31.b0, PRU0_ARM_INTERRUPT+16
    
    // Delay 33 milliseconds to allow sonar to stop resonating and for sound burst to decay in environment
    MOV r0, 3300000
RESET_DELAY_33MS:
    SUB r0, r0, 1
    QBNE RESET_DELAY_33MS, r0, 0
    
    // Jump back to triggering the sonar
    JMP TRIGGER
