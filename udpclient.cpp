#include "udpclient.h"


#include <iostream>
#include <string.h>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "agent.h"


using namespace std;

#define PORT "4950"


udpclient::udpclient(char* p) : port(p) {}
udpclient::~udpclient() {}


void udpclient::setAgent(Agent* a) {myAgent = a;}
Agent*& udpclient::getAgent() {return myAgent;}



struct addrinfo *servinfo;  //will point to the results

bool udpclient::launch_client() {
    int status;

    struct addrinfo hints;

    fd_set read_flags,write_flags; // the flag sets to be used
    struct timeval waitd = {10, 0};          // the max wait time for an event
    int sel;                      // holds return value for select();

    memset(&hints, 0, sizeof hints);    //make sure the struct is empty
    hints.ai_family = AF_INET;          //ipv4
    hints.ai_socktype = SOCK_DGRAM;     //udp

    //get server info, put into servinfo
    if ((status = getaddrinfo("192.168.2.3", PORT, &hints, &servinfo)) != 0) {
        printf("\ngetaddrinfo error: %m", errno);
        return false;
    }

    //make socket
    fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (fd < 0) {
        printf("\nserver socket failure %m", errno);
        return false;
    }

    return true;
}


void udpclient::communicate() {

    while(1) {

        usleep(15000);

        std::ostringstream tosend;
        tosend<<"@ "<<myAgent->getHighSV()<<" "<<myAgent->getLowSV();


        int numSent = sendto(fd, tosend.str().c_str(), tosend.str().length(), 0, servinfo->ai_addr, servinfo->ai_addrlen);
        if(numSent < 0)
            printf("\nError sending %m", errno);
        else
            cout<<"\nBytes Sent - "<<numSent;
    }

}


