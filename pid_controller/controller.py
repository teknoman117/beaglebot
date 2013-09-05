# An object which controls a motor on beaglebot

from eqep import eQEP
from pid import PID

import Adafruit_BBIO.GPIO as gpio
import Adafruit_BBIO.PWM as pwm

import threading

# Motor controller object
class controller(threading.Thread):
    # Instantiate the motor controller
    def __init__(self, pwm_name, dir_a, dir_b, eqep):
        # Initialize the thread base
        super(controller, self).__init__()
        
        # Create stop event object
        self.stop_event = threading.Event()
        self.stop_confirm = threading.Event()
        
        # Store parameters
        self.pwm = pwm_name
        self.dir_a = dir_a
        self.dir_b = dir_b
        
        # Create a PID controller
        self.pid = PID(0.01, 0.01, 0.0, -100.0, 100.0, 0.0)
        
        # Load the eQEP interface
        self.eqep = eQEP(eqep, eQEP.MODE_RELATIVE)
        self.eqep.set_period(10000000)
        
        # Setup the GPIO
        gpio.setup(self.dir_a, gpio.OUT)
        gpio.setup(self.dir_b, gpio.OUT)
        gpio.output(self.dir_a, gpio.LOW)
        gpio.output(self.dir_b, gpio.LOW)
        
        # Setup the PWM
        pwm.start(self.pwm, 100.0)
    
    # Process of the running thing
    def run(self):
        while (self.stop_event.is_set() == False):
            current = float(self.eqep.poll_position())
            output = self.pid.calculate(current)
            
            # Calculate the output duty cycle 
            duty = 100.0 - abs(output)
                
            # Setup the IO lines correctly
            if(output > 0.0):
                gpio.output(self.dir_a, gpio.HIGH)
                gpio.output(self.dir_b, gpio.LOW)
            else:
                gpio.output(self.dir_a, gpio.LOW)
                gpio.output(self.dir_b, gpio.HIGH)
                
            # Set the pwm
            pwm.set_duty_cycle(self.pwm, duty)
            
            # Log
            #print "Current = " + str(current) + "; Output = " + str(output)
        # Confirm stop
        self.stop_confirm.set()
            
    # Stop the motor
    def stop(self):
        self.stop_event.set()
        self.stop_confirm.wait()
        self.stop_event.clear()
        self.stop_confirm.clear()
        gpio.output(self.dir_a, gpio.LOW)
        gpio.output(self.dir_b, gpio.LOW)
        pwm.set_duty_cycle(self.pwm, 100.0)
        
        
