# API for the TI eQEP hardware driver I wrote

# We need OS operations for this
import os, select

class eQEP(object):
    # Modes
    MODE_ABSOLUTE = 0
    MODE_RELATIVE = 1
    
    # eQEP Controller Locations
    eQEP0 = "/sys/devices/ocp.2/48300000.epwmss/48300180.eqep"
    eQEP1 = "/sys/devices/ocp.2/48302000.epwmss/48302180.eqep"
    eQEP2 = "/sys/devices/ocp.2/48304000.epwmss/48304180.eqep"

    # Set the mode of the eQEP hardware
    def set_mode(self, mode):
        # Open the mode attribute file
        attribute = open(self.path + "/mode", "w")
        
        # Write the desired mode into the file
        attribute.write(str(mode))
        
        # Close the file
        attribute.close()
        
    # Get the mode of the eQEP hardware
    def get_mode(self):
        # Open the attribute file
        attribute = open(self.path + "/mode", "r")
        
        # Get the value
        mode = int(attribute.readline())
        
        # Close the attribute
        attribute.close()
        
        # Return the mode
        return mode

    # Set the unit timer period of the eQEP hardware
    def set_period(self, period):
        # Open the mode attribute file
        attribute = open(self.path + "/period", "w")
        
        # Write the desired mode into the file
        attribute.write(str(period))
        
        # Close the file
        attribute.close()
        
    # Get the unit timer period of the eQEP hardware
    def get_period(self):
        # Open the attribute file
        attribute = open(self.path + "/period", "r")
        
        # Get the value
        period = int(attribute.readline())
        
        # Close the attribute
        attribute.close()
        
        # Return the mode
        return period
        
    # Set the current position of the encoder hardware
    def set_position(self, position):
        # Open the mode attribute file
        attribute = open(self.path + "/position", "w")
        
        # Write the desired mode into the file
        attribute.write(str(position))
        
        # Close the file
        attribute.close()
        
    # Get the immediate position of the encoder hardare
    def get_position(self):
        # Open the attribute file
        attribute = open(self.path + "/position", "r")
        
        # Get the value
        position = int(attribute.readline())
        
        # Close the attribute
        attribute.close()
        
        # Return the mode
        return position
        
    # Poll the position, returns when new data is available
    def poll_position(self):
        # Poll the position file
        self.poller.poll(-1)
        
        # Seek to the beginning of the file to get the data
        os.lseek(self.fd, 0, 0)
        
        # Return the position
        return int(os.read(self.fd, 16))
    
    # Constructor - specify the path and the mode
    def __init__(self, path, mode):
        # Base path of the eQEP sysfs entry (ex. /sys/devices/ocp.2/48302000.epwmss/48302180.eqep)
        self.path = path;
        
        # Set the mode
        self.set_mode(mode)

        # Reset the position
        self.set_position(0)
        
        # Setup polling system
        self.fd = os.open(self.path + "/position", os.O_RDONLY, os.O_NONBLOCK)
       
        # Create the poll object
        self.poller = select.poll()
        self.poller.register(self.fd, select.POLLPRI)  
        
    # Deconstructor
    def __del__(self):
        # Cleanup polling system
        self.poller.unregister(self.fd)
        os.close(self.fd)   
        
        
