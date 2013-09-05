# A python implementation of a PID controller

class PID(object):
    # Initialize the PID controller
    def __init__(self, Kp, Ki, Kd, minimum_output, maximum_output, target):
        self.target = target
        self.Kp = Kp
        self.Ki = Ki
        self.Kd = Kd
        self.minimum_output = minimum_output
        self.maximum_output = maximum_output
        self.integral = 0.0
        self.prior = 0.0
        
    # Perform a loop of the PID controller (feed in new input, returns output)
    def calculate(self, actual):
        # Calculate the immediate error
        error = self.target - actual
        
        # Calculate the integral
        self.integral += error * self.Ki
        
        # Clamp the integral to our boundaries
        if(self.integral > self.maximum_output):
            self.integral = self.maximum_output
        if(self.integral < self.minimum_output):
            self.integral = self.minimum_output
        
        # Calculate the derivative
        derivative = actual - self.prior
        
        # Calculate the PID output
        output = (self.Kp * error) + self.integral + (self.Kd * derivative)
        
        # Clamp PID output to bounds
        if(output > self.maximum_output):
            output = self.maximum_output
        if(output < self.minimum_output):
            output = self.minimum_output
        
        # Store the input as the prior
        self.prior = actual
        
        # Return the ouput
        return output
        
        
