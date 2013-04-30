// Simple kernel module to set the pin muxings required by the HC-SR04 demo

#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for the macros */
#include <linux/ioport.h>
#include <asm/io.h>

static int __init io_setmux_start(void)
{
    // stuff
    u32   value;
    void *control_module;
    void *gpio1_controller;

    // Print the loading message
    printk(KERN_INFO "Setting up IO muxes...\n");
    
    // Map the control module to our memory space
    if(!(control_module = ioremap(0x44e10000, 0x2000)))
    {
        printk(KERN_INFO "Unable to map control module\n");
        return -EINVAL;
    }
    
    // Setup conf_gpmc_a6 as an output, no pullups, mux mode 7 = gpio1_6
    iowrite32(0x0000000F, control_module + 0x818);
    
    // Setup conf_gpmc_a7 as an input, pullup resistor enabled, mux mode 7 = gpio1_7
    iowrite32(0x00000037, control_module + 0x81C);
    
    // Release the control module
    iounmap(control_module);
    
    // Map GPIO1 controller to our memory space
    if(!(gpio1_controller = ioremap(0x4804c000, 0x1000)))
    {
        printk(KERN_INFO "Unable to map gpio1\n");
        return -EINVAL;
    }
    
    // Get the existing pin direction configuration
    value = ioread32(gpio1_controller + 0x134);
    
    // Configure gpio1_6 as an output (direction bit = 0)
    value &= ~(1 << 6);
    
    // Configure gpio1_7 as an input (direction bit = 1)
    value |= (1 << 7);
    
    // Write the pin direction configuration back to gpio controller 1
    iowrite32(value, gpio1_controller + 0x134);
    
    // Release the gpio controller
    iounmap(gpio1_controller);
    
    // Alert success
    printk(KERN_INFO "IO mux set complete\n");
    
    // Return sucess
    return 0;
}

static void __exit io_setmux_end(void)
{
    // Do nothing, initialization function does all this module needs to
}

module_init(io_setmux_start);
module_exit(io_setmux_end);

MODULE_DESCRIPTION("kernel module to setup pin muxes");
MODULE_AUTHOR("Nathaniel Lewis <linux.robotdude@gmail.com>");
MODULE_LICENSE("GPL");
