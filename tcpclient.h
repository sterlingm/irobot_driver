

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <cstdlib>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


class Agent;

class TcpClient {
public:
	//! A Constructor
	/*! Sets port to p */
    TcpClient(char*);
    //! A Destructor
    ~TcpClient();

    //! Getter function for myAgent member
    /*! Returns a reference of myAgent member */
    Agent*& getAgent();

    //! Setter function for myAgent member
    /*! Sets myAgent to a */
    void setAgent(Agent*);

    //! Setter function for ip_addr member
    /*! Sets ip_addr to addr*/
    void setIP(char*);

    //! Getter function for ip_addr member
    /*! Returns the value of ip_addr member*/
    char* getIP();

    //! Launches tcp client
    /*!
     * Tries to connect to a tcp server\n
     * Returns true on success and false otherwise
     */
    bool launchClient();

    //! Communicate with server
    /*! Allows the client to send and receive messages from the server */
    void communicate();

    //! Updates the server's agernt
    /*! Sends a message to the server containing the current position and goal */
    void updateServerAgent();

    //! Gets and processes commands
    /*! Takes in commands from the server and act on them */
    void getCommand(char*);

    //! Polls the robot for sensor values
    /*! Polls the robot for sensor values every 15 milliseconds */
    void pollSensor();
private:
    char* ip_addr;
    char* port;
    int fd;
    char* update_agent;
    bool done;
    Agent* myAgent;
};

#endif
