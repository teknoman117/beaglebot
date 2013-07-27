C++ eQEP API
===============

This is the documentation on the C++ bindings for the eQEP driver.  These bindings provide a very simplistic, but complete interface to the driver.  A c++ source file is provided - eqep.cpp, which contains the eQEP class that provides the binding.

A test application is provided as test.cpp.  It sets the polling period of the provided eQEP device to 10 Hz and indefinitely polls the position

Using the C++ API
--------------------

To create an instance of the eQEP object, you must know the root path of the eQEP sysfs entry.  A guide is provided in encoders/README.md.  The following code create an object representing the eQEP1 interface.  The number on ocp.* may change based on hardware and/or software revision, but the numbers on *.epwmss and *.eqep will never change, as they represent the addresses of where the hardware is physically located.  The second parameter of the constructor is the operating mode of the eQEP hardware.  Options are eQEP::eQEP_Mode_Absolute and eQEP::eQEP_Mode_Relative and are described in encoders/README.md.

It is important when building to use the --std=c++0x or --std=c++11 flag, because for some reason cstdint was not provided in earlier releases of C++.

```cpp
// Create an encoder instance for eQEP1 in Absolute mode
eQEP *encoder = new eQEP("/sys/devices/ocp.2/48302000.epwmss/48302180.eqep", eQEP::eQEP_Mode_Absolute);
```

The eQEP driver provides the ability to poll the position attribute.  We need to set how often that poll will return.  The default when the driver loads is 1 second.  To set, call eQEP::set_period(uint64_t nanoseconds).

```cpp
// Set the period to 0.1 seconds, or 100,000,000 nanoseconds
encoder->set_period(100000000L);
```

To read the immediate position of the encoder in absolute mode, or the last recorded position in relative mode, you call eQEP::get_position(bool poll), with poll = false

```cpp
// Get the immediate position
int32_t position = encoder->get_position(false);
```

To poll the position, as in wait for the next available piece of data (as determined by the period value), use eQEP::get_position(bool poll), with poll = true

```cpp
// Loop forever and dump a position when its available
while (1)
{
    std::cout << "Position = " << encoder->get_position(true) << std::endl;
}
```

It is possible to change the mode of the eQEP hardware after the object has been initialized

```cpp
// Set the mode of encoder to relative mode
encoder->set_mode(eQEP::eQEP_Mode_Relative);
```

It is possible to also get the current period and mode values as well
```cpp
// Get the current mode of the encoder
eQEP::eQEP_Mode mode = encoder->get_mode();

// Get the current period of the encoder
uint64_t period = encoder->get_period();
```

Have fun! - Nathaniel Lewis

