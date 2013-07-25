#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <poll.h>

#define EQEP1_POSITION "/sys/devices/ocp.2/48302000.epwmss/48302180.eqep/position"

int main(int argc, char **argv)
{
    char dummy;
    struct pollfd ufd;

    while(1)
    {
        // Open a connection to the attribute file.
        if ((ufd.fd = open((argc > 1) ? argv[1] : EQEP1_POSITION, O_RDWR)) < 0)
        {
            perror("Unable to open notify");
            exit(1);
        }

        // Dummy read
        read(ufd.fd, &dummy, 1);
        
        // Poll the port
        ufd.events = POLLPRI;
        if (poll(&ufd, 1, -1) < 0)
        {
            perror("poll error");
            exit(1);
        }
        
        // Open the file to read the last position
        FILE *fp = fopen(EQEP1_POSITION, "r");

        int32_t val;
        fscanf(fp, "%ld", &val);

        printf("Encoder = %ld\n", val);

        fclose(fp);
        
        close( ufd.fd );
    }
}
