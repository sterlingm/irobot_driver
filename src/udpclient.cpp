#include "udpclient.h"
#include "robot_driver_agent.h"

#include <errno.h>
#include <netdb.h>

struct addrinfo *servinfo;  //will point to the results


/*Constructor and destructor*/
UdpClient::UdpClient(char* p, char* ip, char ID) : port(p), ip_addr(ip), done(false), myAgent(0), id(ID) {}
UdpClient::~UdpClient() {freeaddrinfo(servinfo);}

/*Getter and setter for done*/
void UdpClient::setDone(bool d) {done = d;}
bool UdpClient::getDone() {return done;}

/*Getter and Setter for myAgent*/
void UdpClient::setAgent(Agent* a) {myAgent = a;}
Agent*& UdpClient::getAgent() {return myAgent;}

/*Getter and setter for ip_addr*/
char* UdpClient::getIP() {return ip_addr;}
void UdpClient::setIP(char* addr) {ip_addr = addr;}


/*Launches the client*/
bool UdpClient::launch_client() {
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
void UdpClient::communicate() {

    //message to send
    std::stringstream tosend;
    //hold return value of sendto
    int numSent;

    while(!done) {

        //sleep for 15ms
        usleep(14500);

        //reset tosend
        tosend.str("");

        //grab sensor values
        sensor_packet temp = myAgent->getRobot()->get_sensor_value(myAgent->getCurrentSensor());
        //std::cout<<"\nsensor high byte value: "<<temp.values[1]<<"\n";
        //std::cout<<"\nsensor low byte value: "<<temp.values[0]<<"\n";

        if(temp.values[1] > -256 && temp.values[1] < 256) {
            //put header onto tosend and concatenate the values
            tosend<<"@ "<<myAgent->getRobot()->getID()<<" "<<temp.values[1]<<" "<<temp.values[0];

            //send
            numSent = sendto(fd, tosend.str().c_str(), tosend.str().length(), 0, servinfo->ai_addr, servinfo->ai_addrlen);
            if(numSent < 0)
                printf("\nError sending message: %s\nError message: %m", tosend.str().c_str(), errno);
            //else
                //std::cout<<"\nUDP Sent: "<<tosend.str();
        }   //end if
    }   //end while
}   //END COMMUNICATE


