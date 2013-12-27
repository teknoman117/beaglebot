Patching the Kernel with the eQEP driver
====================================

The eQEP driver is distributed as a kernel source patch, and setting it up is not really that hard.  This guide assumes you are on a Linux system and have a Beaglebone or Beaglebone Black.  If you have never built a Linux kernel for the Beaglebone SBCs before, I'd recommend watching the first 20 minutes or so of Derek Molloy's video on YouTube (http://www.youtube.com/watch?v=HJ9nUqYMjqs).  It provides a great explaination of compiling the kernel for the Beaglebone.  The video is a little dated at this point, as this video was for kernel 3.2.  The general process is the same, but we need a different kernel.

I recommend using the hard float variant of the arm compiler, arm-linux-gnueabihf-gcc, which requires you to use a hard float image of Debian or Ubuntu.  Hard float will give you the best performance because floating point operations are performed on the ARM cpu's FPU instead of being emulated in software.  I developed this driver using the am33x-v3.8 branch of Robert C Nelson's linux-dev repository.

Open a terminal and go to the directory you want to build your kernel in

```bash
git clone https://github.com/RobertCNelson/linux-dev
cd linux-dev
git checkout origin/am33x-v3.8 -b am33x-v3.8
```

Setup your system.sh file as directed in the video.  At this point you will want to build the kernel using 

```bash
./build_kernel.sh
```

Let the kernel complete its building process.  At this point you will want to apply the patch.  The build process will have created a mirror image of the linux kernel source in the linux-dev/KERNEL directory.  Open a terminal and go to this directory.  Once there, apply the patch

```bash
git apply /path/to/beaglebot/encoders/patches/0001-tieqep-driver.patch
```

This will apply the patch to the kernel source.  It performs three changes.  First, it adds the eQEP driver as KERNEL/drivers/misc/tieqep.c.  Second, it adds entries to the Makefile and Kconfig to add a menuconfig option to enable the driver.  Lastly, it patches the am33xx.dtsi device tree source file to add entries under the epwmss definitions for the eQEP hardware.  I chose this because in the chip, the eQEP is part of the "Enhanced PWM Sub System," which provides the clock source.  

After patching the kernel, go back to the linux-dev directory and rebuild the kernel.  Don't use the build_kernel.sh script, it will nuke the patch.  Use the rebuild script in the tools directory

```bash
./tools/rebuild.sh
```

When you get the menuconfig screen for the kernel, you now need to enable the driver.  Go to Device Drivers/Misc Devices and build "EQEP Hardware quadrature encoder controller" into the kernel.  Exit and save the configuration and let the kernel rebuild, it shouldn't take very long, as its only building the driver and rebuilding the device tree blobs.

At this point you have a kernel with the eQEP driver.  Install the kernel onto your beaglebone and boot it up.

Continue to the dts/ directory for information on how to configure, compile, and load the device tree overlays to enable the eQEP hardware.

