#include "udpserver.h"
#include "robot_driver_agent.h"

#include <stdlib.h>
#include <errno.h>
#include <netdb.h>

socklen_t addr_len;

Udpserver::Udpserver(char* p, char* ip, client_info*& c, int& size) : port(p), done(false), ip_addr(ip), clients(c), count(size) {}

Udpserver::~Udpserver() {}


bool Udpserver::getDone() {return done;}
void Udpserver::setDone(bool d) {done = d;}



/*Getter and setter for ip_addr*/
void Udpserver::setIP(char* addr) {ip_addr = addr;}
char* Udpserver::getIP() {return ip_addr;}

/*Launches the udp server*/
bool Udpserver::launch_server() {

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
    if ((status = getaddrinfo((const char*)ip_addr, port, &hints, &servinfo)) != 0) {
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

    freeaddrinfo(servinfo);


    return true;
}   //END LAUNCH_SERVER


client_info* Udpserver::get_clients() {return clients;}
int Udpserver::get_num_clients() {return count;}

char Udpserver::get_client_id(int& fd) {
    for(int i=0;i<count;i++)
        if(clients[i].fd == fd)
            return clients[i].id;
}

client_info& Udpserver::get_client(char& id) {
    for(int i=0;i<count;i++)
        if(clients[i].id == id)
            return clients[i];
}

client_info& Udpserver::get_client(int& fd) {
    for(int i=0;i<count;i++)
        if(clients[i].fd == fd)
            return clients[i];
}


/*Intreprets a message from the client*/
void Udpserver::get_message(char* message, char client_id) {
    std::string temp_message(message);
    //std::cout<<"\nmessage: "<<temp_message;

    if(temp_message[0] == '@') {

        int num_headers = 0;
        for(int i=0;i<temp_message.length() && num_headers < 2;i++)
            if(temp_message[i] == '@')
                num_headers++;

        if(num_headers == 1) {

            int length_of_hsv = 0;

            //isolate the high sensor value
            while(isdigit(temp_message[length_of_hsv+4]) || temp_message[length_of_hsv+4] == '-')
                length_of_hsv++;
            std::string hsv_str(temp_message.substr(4, length_of_hsv));
            //std::cout<<"\nhsv_str: "<<hsv_str;
            int hsv = atoi(hsv_str.c_str());

            length_of_hsv++;

            int length_of_lsv = length_of_hsv+1;
            while(isdigit(temp_message[length_of_lsv]) || temp_message[length_of_lsv] == '-')
                length_of_lsv++;
            std::string lsv_str(temp_message.substr(length_of_hsv+4, length_of_lsv - length_of_hsv+4));
            //std::cout<<"\nlsv_str: "<<lsv_str;
            int lsv = atoi(lsv_str.c_str());

            //lock
            pthread_mutex_lock(&mutex_agent);

            get_client(client_id).agent->setHighSV(hsv);
            get_client(client_id).agent->setLowSV(lsv);

            //unlock
            pthread_mutex_unlock(&mutex_agent);

        }   //end if no stacking
    }   //end if starts with header
}   //END GET_MESSAGE


void Udpserver::communicate() {

    int num_read;
    //11 is the max size (if both sensor value bytes are 3 digits)
    char in[11];

    while(!done) {

        num_read = recvfrom(fd, in, 11 , 0, 0, &addr_len);

        if (num_read < 0 ) {}

        else {

            get_message(in, in[2]);

            //wait and clear in
            usleep(5000);
            memset(&in, 0, 11);
        }   //end else
    }   //end while
    close(fd);
}   //END COMMUNICATE
