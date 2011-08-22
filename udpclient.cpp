#include "udpclient.h"
#include "agent.h"

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



struct addrinfo *servinfo;  //will point to the results


/*Constructor and destructor*/
udpclient::udpclient(char* p) : port(p) {}
udpclient::~udpclient() {}

/*Getter and Setter for myAgent*/
void udpclient::setAgent(Agent* a) {myAgent = a;}
Agent*& udpclient::getAgent() {return myAgent;}

/*Getter and setter for ip_addr*/
char* udpclient::getIP() {return ip_addr;}
void udpclient::setIP(char* addr) {ip_addr = addr;}


/*Launches the client*/
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
    if ((status = getaddrinfo(ip_addr, port, &hints, &servinfo)) != 0) {
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
}   //END LAUNCH_CLIENT


/*Sends messages with sensor info to the sensor every 15ms*/
void udpclient::communicate() {

    //message to send
    std::ostringstream tosend;
    //hold return value of sendto
    int numSent;

    while(1) {

        //sleep
        usleep(15000);

        //reset tosend
        tosend.str("");
        //grab sensor values
        Sensor_Packet temp = myAgent->getRobot()->getSensorValue(myAgent->getRobot()->getCurrentSensor());

        //put header onto tosend and concatenate the values
        tosend<<"@ "<<temp.values[1]<<" "<<temp.values[0];

        //send
        numSent = sendto(fd, tosend.str().c_str(), tosend.str().length(), 0, servinfo->ai_addr, servinfo->ai_addrlen);
        if(numSent < 0)
            printf("\nError sending %m", errno);
        //else
            //  cout<<"\nSent: "<<tosend.str();
    }   //end while
}   //END COMMUNICATE


