#include "udpserver.h"
#include "agent.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "4950"    // the port users will be connecting to


udpserver::udpserver(char* p) : port(p) {}

udpserver::~udpserver() {}


void udpserver::setAgent(Agent* a) {myAgent = a;}
Agent*& udpserver::getAgent() {return myAgent;}


bool udpserver::launch_server() {

    int status;

    struct addrinfo hints;
    struct addrinfo *servinfo;  //will point to the results

    //store the connecting address and size
    struct sockaddr_storage their_addr;
    socklen_t their_addr_size;



    //socket info
    memset(&hints, 0, sizeof hints); //make sure the struct is empty
    hints.ai_family = AF_INET;  //ipv4
    hints.ai_socktype = SOCK_DGRAM; //udp



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

    //allow reuse of port
    int yes=1;
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        printf("\nsetsockopt failure %m", errno);
        return false;
    }

    //bind
    if(bind (fd, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
        printf("\nBind error %m", errno);
        return false;
    }

    return true;
}


void udpserver::get_message(char* message) {

    std::string temp_message = message;
    int length_of_highsv = 0;
    while(isdigit(temp_message[length_of_highsv+2]))
        length_of_highsv++;

    std::string temp_highsv = temp_message.substr(2, length_of_highsv);
    int hsv = atoi(temp_highsv.c_str());


    int length_of_lsv = length_of_highsv+1;

    while(isdigit(temp_message[length_of_lsv]))
        length_of_lsv++;

    std::string temp_lowsv = temp_message.substr(length_of_highsv, length_of_lsv - length_of_highsv);
    int lsv = atoi(temp_lowsv.c_str());

    myAgent->setHighSV(hsv);
    myAgent->setLowSV(lsv);

}


void udpserver::communicate() {

    struct sockaddr_storage their_addr;
    socklen_t their_addr_size;
    socklen_t addr_len;
    int num_read;
    char in[255];

    while(1) {

        num_read = recvfrom(fd, in, 255 , 0, (struct sockaddr *)&their_addr, &addr_len);
        if (num_read < 0 )
            printf("Error recv %m", errno);

        else {
            get_message(in);
            usleep(5000);
            memset(&in, 0, 255);
        }   //end else
    }   //end while
    close(fd);
}
