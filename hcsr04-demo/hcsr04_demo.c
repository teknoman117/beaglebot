// Standard includes
#include <stdio.h>

// PRUSS interface library
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

int main (void)
{
    /* Initialize the PRU */
    printf("Initializing PRU\n");
    tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
    prussdrv_init ();
    
    /* Open PRU Interrupt */
    if(prussdrv_open(PRU_EVTOUT_0))
    {
        // Handle failure
        fprintf(stderr, "prussdrv_open open failed\n");
        return 1;
    }
    
    /* Get the interrupt initialized */
    prussdrv_pruintc_init(&pruss_intc_initdata);
    
    /* Get pointers to PRU local memory */
    void *pruDataMem;
    prussdrv_map_prumem (PRUSS0_PRU0_DATARAM, &pruDataMem);
    unsigned int *pruData = (unsigned int *) pruDataMem;
    
    /* Execute example on PRU */
    printf("Executing sonar pru code\n");
    prussdrv_exec_program (0, "hcsr04_demo.bin");
    
    /* Get 1000 measurements */
    int i = 0;
    while(i++ < 1000)
    {
        // Wait for the PRU interrupt to occur
        prussdrv_pru_wait_event (PRU_EVTOUT_0);
        prussdrv_pru_clear_event (PRU0_ARM_INTERRUPT);
        
        // Print out the distance received from the sonar (sound takes 58.77 microseconds to travel 1 cm at sea level in dry air)
        printf("Distance = %f cm\n", (float) pruData[0] / 58.77);
    }
    
    /* Check if example passed */
    printf("Executed succesfully.\r\n");
    
    /* Disable PRU and close memory mapping*/
    prussdrv_pru_disable (0);
    prussdrv_exit ();
    
    return(0);
    
}
