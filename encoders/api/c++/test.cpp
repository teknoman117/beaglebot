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
    
    // Query some attributes of the setup
    eQEP *eqep1 = new eQEP(argv[1], eQEP::eQEP_Mode_Absolute);
    
    eqep1->set_period(500000000L);
    
    // Print out period
    //std::cout << "[eQEP " << argv[1] << "] Period = " << eqep1->get_period() << std::endl;
    
    // Read position indefintely
    while(1)
    {
        std::cout << "[eQEP " << argv[1] << "] Position = " << eqep1->get_position() << std::endl;
    }
    
    // Derp
    delete eqep1;
    return 0;
}
