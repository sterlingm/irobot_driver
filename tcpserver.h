

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <cstdlib>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


class Agent;
class Path;

class TcpServer {
public:

    //! A Constructor
    /*! Sets port to p */
    TcpServer(char*);
    //! A Destructor
    ~TcpServer();

    //! Getter function for myAgent member
    /*! Returns reference of myAgent member */
    Agent*& getAgent();

    //! Setter function for myAgent member
    /*! Sets myAgent to a */
    void setAgent(Agent*);

    //! Getter function for ip_addr
    /*! Returns the value of ip_addr */
    char* getIP();

    //! Setter function for ip_addr
    /*! Sets ip_addr to addr*/
    void setIP(char*);

    //! Launches tcp server
    /*!
     * Waits for a tcp client to connect\n
     * Returns true on success and false otherwise
     */
    bool launchServer();

    //! Gets message sent back from client
    /*!
     * Receives message from client in response to a previously sent command\n
     * Acts according to the send back
     */
    void getSendBack(char*);

    //! Communicate with client
    /*! Allows the server to send and receive messages from the client */
    void communicate();

    //! Sends updated path to the client
    /*! Sends a message to the client containing the updated path*/
    void sendPath(Path&);


private:
    char* ip_addr;
    char* port;
    int fd;
    int comm_fd;
    bool done;
    Agent* myAgent;
};

#endif
