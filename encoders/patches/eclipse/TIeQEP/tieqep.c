/**
 * TI eQEP driver for AM33xx devices
 *
 * Copyright (C) 2014 Nathaniel R. Lewis - http://nathanielrlewis.com/
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
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/of_device.h>

// Include the configured options for the kernel
#include <generated/autoconf.h>

// Maximum supported eQEP devices
#define NUM_EQEP_UNITS 3

// Driver global state
static struct class *eqep_class;
static        dev_t  baseNode;


// Create an instance of the eQEP driver
static int eqep_probe(struct platform_device *pdev)
{
    // We have successfully setup the eQEP unit
	printk(KERN_INFO "[TIeQEP] Device Probed\n");
    return 0;
}

// Remove an instance of the eQEP driver
static int eqep_remove(struct platform_device *pdev)
{
    // Return success
	printk(KERN_INFO "[TIeQEP] Device Removed\n");
    return 0;
}

// Power management suspend device
static int eqep_suspend(struct device *dev)
{
    // Return success
    return 0;
}

// Power management wake device back up
static int eqep_resume(struct device *dev)
{
    // Success
    return 0;
}

// create pm functions object
static SIMPLE_DEV_PM_OPS(eqep_pm_ops, eqep_suspend, eqep_resume);

// Driver compatibility list
static struct of_device_id eqep_of_match[] =
{
    { .compatible = "ti,am33xx-eqep" },
    { }
};

// Register our compatibilities for device trees
MODULE_DEVICE_TABLE(of, eqep_of_match);

// Platform driver information
static struct platform_driver eqep_driver =
{
    .driver =
    {
        .name           = "eqep",
        .owner          = THIS_MODULE,
        .pm             = &eqep_pm_ops,
        .of_match_table = eqep_of_match,
    },
    .probe = eqep_probe,
    .remove = eqep_remove,
};

// Called when the module is loaded into the kernel
static int __init eqep_init(void)
{
	int error = 0;

	// Create a class for the eqep units
	eqep_class = class_create(THIS_MODULE, "eqep");
	if(IS_ERR(eqep_class))
	{
		return -EINVAL;
	}

	// Allocate some device nodes for use in the driver
	error = alloc_chrdev_region(&baseNode, 0, NUM_EQEP_UNITS, "eqep");
	if(error < 0)
	{
		return -ENOMEM;
	}

	// Register the platform driver
	platform_driver_register(&eqep_driver);

	// Successfully initialized
	printk(KERN_INFO "[TIeQEP] Module Loaded\n");
	return 0;
}

// Called when the module is removed from the kernel
static void __exit eqep_exit(void)
{
	// Release the allocated device node ids
	unregister_chrdev_region(baseNode, NUM_EQEP_UNITS);

	// Destroy the class
	class_destroy(eqep_class);

	// Unregister the platform driver
	platform_driver_unregister(&eqep_driver);

	// Successfully removed
	printk(KERN_INFO "[TIeQEP] Module Exited\n");
}

// Tell the compiler which functions are init and exit
module_init(eqep_init);
module_exit(eqep_exit);

// Module information
MODULE_DESCRIPTION("TI eQEP driver");
MODULE_AUTHOR("Nathaniel R. Lewis");
MODULE_LICENSE("GPL");
