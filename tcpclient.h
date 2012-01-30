

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
class Grid;

class TcpClient {
public:
	//! A Constructor
	/*! Sets port to p, ip_addr to ip, and done to false */
    TcpClient(char*, char*, char);
    //! A Destructor
    ~TcpClient();


    //! Getter function for done member
    /*! Returns the value of done member */
    bool getDone();

    //! Setter function for done member
    /*! Sets done to d */
    void setDone(bool);

    //! Getter function for read_messages member
    /*! Returns the value of the read_mess */
    bool get_read_mess();

    //! Setter function for read_messages member
    /*! Sets read_mess to r */
    void set_read_mess(bool);

    //! Getter function for sent_messages member
    /*! Returns the value of sent_mess member */
    bool get_sent_mess();

    //! Setter function for sent_messages member
    /*! Sets sent_mess to s */
    void set_sent_mess(bool);


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
    char id;
    int fd;
    bool done;
    bool read_mess;
    bool sent_mess;
    Agent* myAgent;
    Grid* grid;     //needed to persist myAgent's grid member

    void send_init_info();
    void receive_grid_filename();
    void print_message(char*);
};

#endif
