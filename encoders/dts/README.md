eQEP Device Tree Overlays
=========================

The device tree has existed in the kernel for a few years now to aid in creating drivers for new hardware, but allowing new devices to reuse all existing code.  In the ARM space, they were practically ignored.  With the surge of new ARM boards coming out, every vendor had a branch of the kernel dedicated to their board.  Linus Torvalds decided enough was enough and demanded that the ARM people adopt the device tree to keep the influx of changes to the kernel at a sane level.

Starting with kernel 3.8, ARM Linux uses the device tree and makes use of device tree overlays.  These are loaded in runtime to augment to configuration of a board while the kernel is booted.  This makes configuration of IO more friendly because you don't have to recompile your board's base overlay. Since overlays are applied during runtime, you don't have to reboot either.  Three device tree overlays have been provided, one each for eQEP0, eQEP1, and eQEP2.

Device trees and device tree overlays have become the standard for the Linux kernel, therefore the driver depends on the device tree, requiring kernel 3.8+ to be used, which is available out of the box with Angstrom, Debian, Ubuntu, and others.


eQEP driver configuration
-------------------------
Each device tree overlay contains a few settings for configuring the eQEP hardware.  If any of these fields are not specified, the driver falls back on the default value for that field.

__count_mode__ - Sets the counting mode of the eQEP unit.  Mode 0 (default) is for standard quadrature encoder pulse train. see (http://en.wikipedia.org/wiki/File:Quadrature_Diagram.svg).  Mode 1 is a bit more rare.  The encoder produces a clock and a direction signal.  eQEPx_chA becomes the encoder clock input and eQEPx_chb becomes the direction input.  

__swap_inputs__ - Setting to zero (default) has no effect on the inputs.  Setting to 1 causes the eQEP hardware to switch the channel a and b inputs

__invert_qa__ - Setting to zero (default) has no effect on the input.  Setting to 1 inverts the channel a input

__invert_qb__ - Setting to zero (default) has no effect on the input.  Setting to 1 inverts the channel b input

__invert_qi__ - Setting to zero (default) has no effect on the input.  Setting to 1 inverts the index signal input

__invert_qs__ - Setting to zero (default) has no effect on the input.  Setting to 1 inverts the strobe signal input

Note on index and strobe inputs: At the moment, the driver does not provide any support for them.  They each can be used for two purposes.  Both inputs support latching the position to an internal register on the rising or falling edge of the input signal and both provide an option to reset the position counter when they are triggered as well.  TI mentions that this is useful because some encoders that are used for position sensing provide an index output, which is triggered once per complete rotation of the encoder.  At the moment, I have offered no support for these inputs.  If this becomes something that the community desires, by all means shoot me a message and i'll work on it.

Loading a Device Tree Overlay
-------------------------------------------

The dtbo/ sub directory contains several precompiled device tree overlays.  They are the compiled versions of the .dts files in this directory.  The dts files are human-readable and if the settings are not what your application requires, read the "Compiling a Device Tree Overlay" section

Copy the bone_eqepX-00A0.dtbo files to the /lib/firmware directory on the beaglebone.  To load a device tree overlay, for instance eqep1

```bash
echo bone_eqep1 > /sys/devices/bone_capemgr.*/slots
```

The eQEP driver will now load for eQEP 1 and the sysfs interface will be registered.

You can now proceed over to the api/ folder and try out some demos of the driver

Compiling a Device Tree Overlay
-------------------------------------------

First you need to fetch a version of the device tree compiler that supports overlays.  The current Ubuntu and Debian device-tree-compiler packages do not support this.  The following script only works on Debian and its derived distributions, such as Ubuntu.

```bash
wget -c https://raw.github.com/RobertCNelson/tools/master/pkgs/dtc.sh
chmod +x dtc.sh
./dtc.sh
```

To compile one of the provided device tree overlays, for instance bone_eqep1

```bash
dtc -O dtb -o bone_eqep1-00A0.dtbo -b 0 -@ bone_eqep1.dts
```

