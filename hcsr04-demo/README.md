hcsr04-demo
==========

This is an example of using the Beaglebone with an HC-SR04 sonar.  It takes advantage of the Beaglebone's PRU, or Programmable Realtime Unit.  These are a pair of 200 MHz microcontroller cores which execute exactly one intruction per clock cycle, making it easy to write software that depends on realtime hardware.  With the PRU, you can achieve around 10ns measurement accuracy.  In this case, I am using it for the timing of an hc-sr04's echo pulse.  The core triggers a pulse and then measures the echo.  It then triggers a special interrupt to alert the CPU that it has received a ping.  A program running under Linux then catches this event and does something about it.  In this case, prints the measurement to the screen. 

This example assumes you have downloaded and compiled the assembler and library contained here: https://github.com/beagleboard/am335x_pru_package.  Copy the shared library libprussdrv.so into your library path and copy the headers into your header path.  This example uses GPIO1_6 for the trigger pulse, which needs to be configured as an output, and uses GPIO1_7 for the inverted echo pulse, which needs to be configured as an input with the pullup resistor enabled.  Here's a picture of the schematic: http://teknoman117.files.wordpress.com/2013/04/bb-hcsr04-circuit-diagram.jpg.  This circuit protects your beaglebone from the 5V of the HC-SR04 by using echo to drive an NPN transistor connected between an IO line and ground.  Note that this inverts the echo.  Logical low means we're in the echo cycle. 3.3V is enough to trigger the sonar pulse, so that line can be directly connected to the beaglebone.  The capacitor is in place because when I was using my wallwart, apparently the power was a little noisy so the sonar would malfunction.  There are some high power tools used in my lab building.

Just open a terminal and navigate to the hcsr04-demo directory and run make.  I don't have cross compilation set up here, it was meant to run from the beaglebone.  Then run the generated executable "hcsr04_demo" as root, or you will encounter a segmentation fault.  If everything was setup right, the program will load the generated pru binary (hcsr04_demo.bin, should be in the directory you run the application from) into the PRU and then wait for events from the PRU and print out the distance in centimeters.

Note about kernel 3.6 and 3.7
----
Since kernel 3.6, the omap_mux kernel driver was removed in favor of the new device tree system of hardware configuration.  It's an efficient system, but it requires editing files that are part of the board's firmware.  It's not compilicated, but it was nice with the omap_mux driver to be able to change the pin muxing of the board at runtime.  If you have your kernel source lying around, I have included a kernel module (hcsr04-pinmux-set) that will setup the IO pins mentioned above to the right configuration.  Open a terminal in the hcsr04-pinmux-set directory and run "make KDIR=<kernel source directory> COMPILER_PREFIX=<prefix of your cross compiler>"  For example, on my dev machine I run "make KDIR=/home/nathaniel/Programming/beaglebot-kernel/KERNEL COMPILER_PREFIX=arm-cortex_a8-linux-gnueabi-".  Just copy the generated hcsr04_pinmux_set.ko to your beaglebone and run "insmod hcsr04_pinmux.ko" as root.  Just check your dmesg to see if it succeeded.

Note about kernel 3.8+
----
Since kernel 3.8, we have device tree overlays.  These are loaded in runtime to augment to configuration of a board while the system is booted.  This makes the device tree system a lot more friendly because you don't have to recompile the board's base overlay, and since it's during runtime, you don't have to reboot either.  A device tree overlay has been provided (note: untested at the moment) which enabled the pruss subsystem and also sets up the pins for this demo.  To compile it, grab and install a copy of the device tree compiler (script only works on Debian and Debian derived systems such as Ubuntu)

```bash
wget -c https://raw.github.com/RobertCNelson/tools/master/pkgs/dtc.sh
chmod +x dtc.sh
./dtc.sh
````

And then execute the following command

```bash
dtc -O dtb -o hcsr04_demo-00A0.dtbo -b 0 -@ hcsr04_demo.dts
```

Copy the resulting hcsr04_demo-00A0.dtbo to /lib/firmware, and then run the following on your beaglebone

```bash
echo hcsr04_demo > /sys/devices/bone_capemgr.*/slots
```

This will load the overlay and you should see some output in dmesg.  Congradulations - you've just loaded a device overlay and the demo pin configuration is locked in!

