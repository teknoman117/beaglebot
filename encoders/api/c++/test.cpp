#include "eqep.h"

int main (int argc, char** argv)
{
    // If argc less than two, whoops
    if(argc < 2)
    {
        // Print out a usage example
        std::cerr << "Usage: " << argv[0] << " <root path of eQEP sysfs entry>.  e.g. " << argv[0] << " /sys/devices/ocp.2/48302000.epwmss/48302180.eqep" << std::endl;
        
        // Exit
        return 1;
    }
    
    // Allocate an instane of 
    eQEP eqep1(argv[1], eQEP::eQEP_Mode_Absolute);
    
    // Set the unit time period to 100,000,000 ns, or 0.1 seconds
    eqep1.set_period(100000000L);
    
    // Query back the period
    std::cout << "[eQEP " << argv[1] << "] Period = " << eqep1.get_period() << " ns" << std::endl;
    
    // Read position indefintely
    while(1)
    {
        std::cout << "[eQEP " << argv[1] << "] Position = " << eqep1.get_position() << std::endl;
    }
    
    // Return success
    return 0;
}
