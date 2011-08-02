

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
using namespace std;


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

    //! Getter function for update_agent member
    /*! Returns a reference of update_agent member */
    char*& get_agent_update();

    //! Launches client
    /*!
     * Tries to connect to a server\n
     * Program exits if any step of connecting goes wrong
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

private:
    char* port;
    int fd;
    char* update_agent;
    bool done;
    Agent* myAgent;
};

#endif
