/*
 * Beaglebot Motor Driver
 *
 * Copyright (C) 2013 Nathaniel Lewis.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/pm_runtime.h>
#include <linux/pinctrl/consumer.h>
#include <linux/of_gpio.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/interrupt.h>
#include <linux/atomic.h>

// constant timer expiry
//const ktime_t pid_interval = { 100000000L };
const ktime_t pid_interval = { 10000000L };

// Beaglebot motor instance definition
struct beaglebot_motor 
{
    // Platform device
    struct platform_device *pdev;

    // PWM channel for the device
    struct pwm_device *pwm;
	
	// Motor control GPIOs
    int motor_dirA;
    int motor_dirB;
	
	// Encoder GPIOs and their interrupts
    int encoder_chA;
    int encoder_chB;
    int encoder_chA_irq;
    int encoder_chB_irq;
	
	// PID loop information
    struct hrtimer pid_timer;
    struct work_struct pid_task;
    
    unsigned long  target;
    unsigned long  actual;
	atomic64_t     ticks;
	
	// Flags
    int pwm_requested;
    int r;
    int d;
};

// Driver compatibility list
static struct of_device_id beaglebot_motor_of_match[] = 
{
	{ .compatible = "bone-cape-beaglebot-motors" },
	{ }
};

// Register our compatibilities for device trees
MODULE_DEVICE_TABLE(of, beaglebot_motor_of_match);

// PID task
void pid_handler (struct work_struct *work)
{
    // Cast the long to a pointer
    struct beaglebot_motor *motor = container_of(work, struct beaglebot_motor, pid_task);
    
    // Fuck with PWM
    if(motor->d == 0)
    {
        motor->r += 200;
        if(motor->r >= 20000) motor->d = 1;
    } else 
    {
        motor->r -= 200;
        if(motor->r <= 0) motor->d = 0;
    }
    
    // store the updated pwm
    pwm_config(motor->pwm, motor->r, 20000);
}

// timer handler to trigger PWM task
enum hrtimer_restart timer_handler(struct hrtimer *timer)
{
    // Get information about us
    struct beaglebot_motor *motor = container_of(timer, struct beaglebot_motor, pid_timer);

    // Advance next expiration
    hrtimer_forward_now(timer, pid_interval);
    
    // Schedule a tasklet
    schedule_work(&motor->pid_task);
    
    // Return that we should loop
    return HRTIMER_RESTART;
}

// Encoder channel A IRQ
static irqreturn_t beaglebot_motor_cha_irq_handler(int irq, void *dev_id)
{
    // Get the instance information
    struct platform_device *pdev = dev_id;
    struct beaglebot_motor *motor = platform_get_drvdata(pdev);

    // Get the two GPIO pins
    if(gpio_get_value(motor->encoder_chA) ^ gpio_get_value(motor->encoder_chB))
    {
        atomic64_dec(&motor->ticks);
    } else
    {
        atomic64_inc(&motor->ticks);         
    }
	
	// Return that the IRQ was handled successfully
	return IRQ_HANDLED;
}

// Encoder channel B IRQ
static irqreturn_t beaglebot_motor_chb_irq_handler(int irq, void *dev_id)
{
    // Get the instance information
    struct platform_device *pdev = dev_id;
    struct beaglebot_motor *motor = platform_get_drvdata(pdev);

    // Get the two GPIO pins
    if(gpio_get_value(motor->encoder_chA) ^ gpio_get_value(motor->encoder_chB))
    {
        atomic64_inc(&motor->ticks);
    } else
    {
        atomic64_dec(&motor->ticks);         
    }

    // Return that the IRQ was handled successfully
    return IRQ_HANDLED;
}

// Function to read the current target of the motor
static ssize_t beaglebot_motor_get_target(struct device *dev, struct device_attribute *attr, char *buf)
{
     // Get the instance structure
	struct beaglebot_motor *motor = dev_get_drvdata(dev);
	
	// Return the target in string format
	return sprintf(buf, "%lu\n", motor->target);
}

// Function to set the current target of the motor
static ssize_t beaglebot_motor_set_target(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
     // Get the instance structure
	int rc;
	struct beaglebot_motor *motor = dev_get_drvdata(dev);

     // Convert the input string to a long int
	if ((rc = kstrtoul(buf, 0, &motor->target)))
		return rc;
	
	// Fudged -> set the pwm to the motor target
	if ((rc = pwm_config(motor->pwm, motor->target, 20000))) {
		pr_err("Unable to set target. Err %d, (duty) = (%lu)\n", rc, motor->target);
		return rc;
	}
	return count;
}

// Function to read the current actual velocity of the motor
static ssize_t beaglebot_motor_get_actual(struct device *dev, struct device_attribute *attr, char *buf)
{
     // Get the instance structure
	struct beaglebot_motor *motor = dev_get_drvdata(dev);
	
	// Return the target in string format
	return sprintf(buf, "%lld\n", atomic64_read(&motor->ticks));
}

// dummy function to handle write operation to actual velocity
static ssize_t beaglebot_motor_set_actual(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	return count;
}

// Bind read/write functions to sysfs entris
static DEVICE_ATTR(target, 0644, beaglebot_motor_get_target, beaglebot_motor_set_target);
static DEVICE_ATTR(actual, 0644, beaglebot_motor_get_actual, beaglebot_motor_set_actual);

// Array holding all of the sysfs entries
static const struct attribute *beaglebot_motor_attrs[] = {
	&dev_attr_target.attr,
	&dev_attr_actual.attr,
	NULL,
};

// Driver function group
static const struct attribute_group beaglebot_motor_device_attr_group = {
	.attrs = (struct attribute **) beaglebot_motor_attrs,
};

// Create the sysfs group for the platform device
static int __init beaglebot_motor_class_init(struct device *dev)
{
	if (sysfs_create_group(&dev->kobj, &beaglebot_motor_device_attr_group))
		return 1;
	return 0;
}

// Function used to initialize an instance of the platform device
static int beaglebot_motor_probe(struct platform_device *pdev)
{
    // Variables
    struct beaglebot_motor *motor;
    struct pinctrl         *pinctrl;
    struct device_node     *pnode = pdev->dev.of_node;

    // Check to see if pins 
    pinctrl = devm_pinctrl_get_select_default(&pdev->dev);
    if (IS_ERR(pinctrl))
        dev_warn(&pdev->dev, "IO not configured in device tree\n");

    // Allocate memory for the instance information
    if (!(motor = devm_kzalloc(&pdev->dev, sizeof(struct beaglebot_motor), GFP_KERNEL))) 
    {
        dev_err(&pdev->dev, "memory error\n");
        return -ENOMEM;
    }

    // Relate the instance information to the platform device
    platform_set_drvdata(pdev, motor);
    motor->pdev = pdev;

    // Initialize motor structure
    motor->r = 0;
    motor->d = 0;
    motor->target = 0;
    atomic64_set(&motor->ticks, 0);

    // Register controls to sysfs
    if (beaglebot_motor_class_init(&pdev->dev)) 
    {
        dev_err(&pdev->dev, "sysfs creation failed\n");
        return -EINVAL;
    }

    // Get the pwm channel to be used by the motor
    motor->pwm = devm_pwm_get(&pdev->dev, NULL);
    if (IS_ERR(motor->pwm)) 
    {
        // Handle if it failed
        dev_err(&pdev->dev, "unable to request PWM\n");	
        motor->pwm_requested = 0;
        return -EINVAL;			
    }
    motor->pwm_requested = 1;

    // Start PWM controller (to zero)
    pwm_config(motor->pwm, 0, 20000);
    pwm_set_polarity(motor->pwm, 0);
    pwm_enable(motor->pwm);

    // Get GPIO
    motor->motor_dirA = of_get_named_gpio(pnode, "direction-a", 0);
    motor->motor_dirB = of_get_named_gpio(pnode, "direction-b", 0);
    motor->encoder_chA = of_get_named_gpio(pnode, "encoder-a", 0);
    motor->encoder_chB = of_get_named_gpio(pnode, "encoder-b", 0);

    // List the GPIO
    printk(KERN_ALERT "IO {dA, dB, eA, eB} = (%d, %d, %d, %d)\n", motor->motor_dirA, motor->motor_dirB, motor->encoder_chA, motor->encoder_chB);

    // Request GPIO
    if (gpio_request_one(motor->motor_dirA, GPIOF_DIR_OUT, "motor_dirA"))
        dev_err(&pdev->dev, "failed to request direction-a\n");
    if (gpio_request_one(motor->motor_dirB, GPIOF_DIR_OUT, "motor_dirB"))
        dev_err(&pdev->dev, "failed to request direction-b\n");
    if (gpio_request_one(motor->encoder_chA, GPIOF_DIR_IN, "encoder_chA"))
        dev_err(&pdev->dev, "failed to request encoder-a\n");
    if (gpio_request_one(motor->encoder_chB, GPIOF_DIR_IN, "encoder_chB"))
        dev_err(&pdev->dev, "failed to request encoder-b\n");

    // Register encoder irqs
    if (IS_ERR_VALUE((motor->encoder_chA_irq = gpio_to_irq(motor->encoder_chA))))
        dev_err(&pdev->dev, "unable to get event GPIO IRQ\n");
    if (IS_ERR_VALUE((motor->encoder_chB_irq = gpio_to_irq(motor->encoder_chB))))
        dev_err(&pdev->dev, "unable to get event GPIO IRQ\n");

    // Attach interrupt service routines
    if (request_irq(motor->encoder_chA_irq, beaglebot_motor_cha_irq_handler, IRQF_TRIGGER_RISING, "encoder_chA_irq", pdev))
        dev_err(&pdev->dev, "unable to request irq for chA\n");

    if (request_irq(motor->encoder_chB_irq, beaglebot_motor_chb_irq_handler, IRQF_TRIGGER_RISING, "encoder_chB_irq", pdev))
        dev_err(&pdev->dev, "unable to request irq for chB\n");
        
    // Create the PID work queue object
    INIT_WORK(&motor->pid_task, pid_handler);

    // Create the timer to regularly execute the PID loop (10 Hz)
    hrtimer_init(&motor->pid_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    motor->pid_timer.function = &timer_handler;
    hrtimer_start(&motor->pid_timer, pid_interval, HRTIMER_MODE_REL);

    // Return success
    return 0;
}

// Remove an instance of beaglebot motor
static int beaglebot_motor_remove(struct platform_device *pdev)
{
    // Get the instance information from the platform device structure
    struct beaglebot_motor *motor = platform_get_drvdata(pdev);

    // Report that the motor has been successfully removed
    dev_info(&pdev->dev, "Removing beaglebot motor driver...\n");

    // Destroy pid interval timer
    hrtimer_cancel(&motor->pid_timer);
    
    // Cancel work
    cancel_work_sync(&motor->pid_task);

    // Unregister sysfs calls
    sysfs_remove_group(&pdev->dev.kobj, &beaglebot_motor_device_attr_group);

    // Disable and release PWM
    if(motor->pwm_requested)
    {
        // Turn off PWM
        pwm_config(motor->pwm, 0, 20000);

        // Disable the pwm controller
        pwm_disable(motor->pwm);

        // Free pwm
        pwm_free(motor->pwm);
    }

    // release irqs
    free_irq(motor->encoder_chA_irq, pdev);
    free_irq(motor->encoder_chB_irq, pdev);

    // release gpio
    gpio_free(motor->motor_dirA);
    gpio_free(motor->motor_dirB);
    gpio_free(motor->encoder_chA);
    gpio_free(motor->encoder_chB);

    // Return success
    return 0;
}

// Platform driver initialization/removal functions
struct platform_driver beaglebot_motor_driver = {
	.probe		= beaglebot_motor_probe,
	.remove		= beaglebot_motor_remove,
	.driver = {
		.name		  = "bone-cape-beaglebot-motors",
		.owner		  = THIS_MODULE,
		//.pm		       = BONEGEIGER_PM_OPS,
		.of_match_table  = beaglebot_motor_of_match,
	},
};

// Register our platform driver
module_platform_driver(beaglebot_motor_driver);

// Register developer and license info
MODULE_AUTHOR("Nathaniel Lewis");
MODULE_DESCRIPTION("Beaglebot Motor Cape");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:bone-cape-beaglebot-motors");
