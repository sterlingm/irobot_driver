#include "udpserver.h"
#include "agent.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>



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
    if ((status = getaddrinfo(IP_ADDR, port, &hints, &servinfo)) != 0) {
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
    //std::cout<<"\nmessage: "<<temp_message;
    int length_of_hsv = 0;

    while(isdigit(temp_message[length_of_hsv+2]) || temp_message[length_of_hsv+2] == '-')
        length_of_hsv++;
    std::string hsv_str= temp_message.substr(2, length_of_hsv);
    //std::cout<<"\nhsv_str: "<<hsv_str;
    int hsv = atoi(hsv_str.c_str());

    length_of_hsv++;

    int length_of_lsv = length_of_hsv+1;
    while(isdigit(temp_message[length_of_lsv]) || temp_message[length_of_lsv] == '-')
        length_of_lsv++;
    std::string lsv_str = temp_message.substr(length_of_hsv+2, length_of_lsv - length_of_hsv+2);
    //std::cout<<"\nlsv_str: "<<lsv_str;
    int lsv = atoi(lsv_str.c_str());

    //std::cout<<"\nhsv: "<<hsv;
    //std::cout<<"\nlsv: "<<lsv;

    //lock
    pthread_mutex_lock(&mutex_agent);

    //set sensor value
    myAgent->setHighSV(hsv);
    myAgent->setLowSV(lsv);

    //unlock
    pthread_mutex_unlock(&mutex_agent);
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
            int term = 6;
            while(isdigit(in[term]) || in[term] == '-')
                term++;
            in[term] = '\0';
            //std::cout<<"\nRead: "<<in;
            get_message(in);
            usleep(5000);
            memset(&in, 0, term);
        }

    }
    close(fd);
}
