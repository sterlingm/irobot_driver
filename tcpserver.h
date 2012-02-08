

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <cstdlib>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>
#include "robot_driver_utility.h"


class Agent;
class Path;

class TcpServer {
public:


    //! A Constructor
    /*! Sets port to p, ip_addr to ip, which_display to 1, and done to false */
    TcpServer(char*, char*, int);
    //! A Destructor
    ~TcpServer();

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


    //! Getter function for which_display member
    /*! Returns the value of which_display member */
    char getWhichDisplay();


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
     * Takes in the string to the grid filename to pass to clients after they connect
     */
    bool launchServer(std::string);

    //! Gets message sent back from client
    /*!
     * Receives message from client in response to a previously sent command\n
     * Acts according to the send back
     */
    void getSendBack(char*, char);

    //! Communicate with client
    /*! Allows the server to send and receive messages from the clien*/
    void communicate();

    //! Sends updated path to the client
    /*! Sends a message to the client containing the updated path*/
    void sendPath(Path&, char);

    //! Getter for clients member
    /*! Returns a reference of clients member*/
    client_info*& get_clients();

    //! Getter for size of clients member
    /*! Returns the value of the size of clients member*/
    const int get_num_clients();

    //! Gets the file descriptor of a client given an id
    /*! Returns a reference of the fd member of the client with given id*/
    int& get_client_fd(char&);

    //! Gets the id of a client give a file descriptor
    /*! Returns a reference of the id member of the client with given file descriptor*/
    char& get_client_id(int&);

    //! Returns client_info struct with given id
    /*! Returns a reference to the client_info with given id*/
    client_info& get_client(char&);

    //! Returns client_info struct with given file descriptor
    /*! Returns a reference to the client_info with given file descriptor*/
    client_info& get_client(int&);

    //! Returns true if a client exists with given id
    bool is_id(char&);

    //! Sets each client_info array's agent members
    /*! Sets clients[i] to &a_vec[i]*/
    void set_agents(std::vector<Agent>&);

private:

    char* ip_addr;
    char* port;
    int fd;
    client_info* clients;
    const int count;
    char which_display;
    bool done;
    bool read_mess;
    bool sent_mess;

    //current sensor of each robot
    std::vector<int> cs;
    //velocities of each robot
    std::vector<int> velocities;
    //algorithms for each robot
    std::vector<int> algos;

    void recv_client_init_info(int&);
    void send_grid_filename(int&, std::string);
};

#endif
