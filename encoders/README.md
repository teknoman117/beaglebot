encoders
========

Driver and API for the Enhanced Quadrature Encoder Pulse (eQEP) decoder found on the AM33xx series TI SoCs.  Designed with mainly the Beaglebone and Beaglebone Black in mind.  The driver registers a sysfs entry with four main attributes.

__enabled__ - turns the eQEP hardware on or off. 0 - off, 1 - on

__mode__ - the capture mode of the eQEP hardware. 0 - Absolute mode.  The position starts at zero and is incremented or decremented by the encoder's movement. 1 - Relative mode.  The position is reset when the unit timer overflows.  This allows for tracking of the encoders movement per unit time.  Useful for velocity regulation

__period__ - the time in nanoseconds for the unit timer to overflow.  The internal clock to the eQEP hardware runs at 100 MHz.  Since the unit timer is a 32 bit timer, this puts the maximum value of this parameter at around 40 seconds.  Note the value for this would be 40,000,000,000 nanoseconds.

__position__ - the position of the encoder.  In absolute mode, this attribute represents the current position of the encoder.  In relative mode, this attribute represents the position of the encoder at the last unit timer overflow.  This attribute is pollable. In either operating mode, the unit timer overflow event wakes up any process polling on this attribute.  This feature is most useful in relative mode.  It allows the process or thread to awake when there is measurement available in the last time period.

Information on configuring the driver in the device tree is presented in the dts directory

The paths for the sysfs entries on the beaglebones are

/sys/devices/ocp.*/{epwmss_addr}.epwmss/{eqep_addr}.eqep/

For eQEP0, the epwmss address is 48300000 and the eqep address is 48300180

For eQEP1, the epwmss address is 48302000 and the eqep address is 48302180

For eQEP2, the epwmss address is 48304000 and the eqep address is 48304180

It is important to note that only eQEP1 and eQEP2 are available on the Beaglebone, and that all three are available on the Beaglebone Black.  This is because one of the encoder clock input pins is not broken out for eQEP0 on the original Beaglebone.

If using eQEP 1 and/or 2, video must be disabled on the Beaglebone and Beaglebone Black, as they share pins with the lcd_data lines.  eQEP 0 on the Beaglebone black can be used with video enabled.

patches
-------

Patches to the kernel source to add the eQEP driver.  README.md contains instructions for building the kernel with the eQEP driver

dts
---

Device Tree Overlays for configuring the eQEP driver and allocating the pins for the eQEP hardware

api 
---

C++ and Python API for the eQEP driver
