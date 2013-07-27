eQEP Device Tree Overlays
=========================

Since kernel 3.8, we have something called a device tree overlay.  These are loaded in runtime to augment to configuration of a board while the kernel is booted.  This makes configuration of IO more friendly because you don't have to recompile your board's base overlay. Since overlays are applied during runtime, you don't have to reboot either.  Three device tree overlays have been provided, one each for eQEP0, eQEP1, and eQEP2.

As this is the new method that will used from now on, the driver at the moment explicitly depends on the device tree, requiring kernel 3.8+ to be used, which is available out of the box with Angstrom, Debian, Ubuntu, and others.

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

Compiling and Loading a Device Tree Overlay
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

Copy the resulting bone_eqep1-00A0.dtbo to /lib/firmware, and then run the following on your beaglebone

```bash
echo bone_eqep1 > /sys/devices/bone_capemgr.*/slots
```

You can now proceed over to the api/ folder and try out some demos of the driver
