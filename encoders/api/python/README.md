Python eQEP API
===============

This is the documentation on the Python bindings for the eQEP driver.  These bindings provide a very simplistic, but complete interface to the driver.  A python source file is provided - eqep.py, which contains the eQEP class that provides the binding.

A test application is provided as test.py.  It sets the polling period of eQEP1 to 10 Hz and indefinitely polls the position

Using the Python API
--------------------

To create an instance of the eQEP object, you must know the root path of the eQEP sysfs entry.  A guide is provided in encoders/README.md.  The following code create an object representing the eQEP1 interface.  The number on ocp.* may change based on hardware and/or software revision, but the numbers on *.epwmss and *.eqep will never change, as they represent the addresses of where the hardware is physically located.  The second parameter of the constructor is the operating mode of the eQEP hardware.  Options are eQEP.MODE_ABSOLUTE and eQEP.MODE_RELATIVE and are described in encoders/README.md.

```python
from eqep import eQEP

# Create an encoder instance for eQEP1 in Absolute mode
encoder = eQEP("/sys/devices/ocp.2/48302000.epwmss/48302180.eqep", eQEP.MODE_ABSOLUTE)
```

The eQEP driver provides the ability to poll the position attribute.  We need to set how often that poll will return.  The default when the driver loads is 1 second.  To set, call eQEP.set_period(nanoseconds).

```python
# Set the period to 0.1 seconds, or 100,000,000 nanoseconds
encoder.set_period(100000000)
```

To read the immediate position of the encoder in absolute mode, or the last recorded position in relative mode, you call eQEP.get_position()

```python
# Get the immediate position
position = encoder.get_position()
```

To poll the position, as in wait for the next available piece of data (as determined by the period value), use eQEP.poll_position()

```python
# Loop forever and dump a position when its available
while True:
    print encoder.poll_position()
```

It is possible to change the mode of the eQEP hardware after the object has been initialized

```python
# Set the mode of encoder to relative mode
encoder.set_mode(eQEP.MODE_RELATIVE)
```

It is possible to also get the current period and mode values as well
```python
# Get the current mode of the encoder
mode = encoder.get_mode()

# Get the current period of the encoder
period = encoder.get_period()
```

Have fun! - Nathaniel Lewis

