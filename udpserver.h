#ifndef UDPSERVER_H
#define UDPSERVER_H
#include "robot_driver_utility.h"


class UdpServer {
public:

    //! Constructor
    /*! Constructor\nSets port to p, ip_addr to ip, and done to false*/
    UdpServer(char*, char*, client_info*&, int&);
    //! Destructor
    /*! Destructor */
    ~UdpServer();

    //! Getter function for done member\nReturns true if the program is done
    bool getDone();
    //! Setter function for done member
    void setDone(bool);

    //! Setter function for ip_addr member
    /*! Sets ip_addr to addr*/
    void setIP(char*);

    //! Getter function for ip_addr member
    /*! Returns the value of ip_addr*/
    char* getIP();

    //! Launches udp server
    /*!
     * Tries to create a udp server\n
     * Returns true on success and false otherwise
     */
    bool launch_server();

    //! Interprets a message from the client
    /*! Takes in a char* that is sent from the client and gets the sensor values from that message */
    void get_message(char*, char);

    //! Receives messages from the client
    /*! Waits for the client to send a message */
    void communicate();

    //! Getter function for clients member
    client_info* get_clients();

    //! Returns the number of clients
    int get_num_clients();

    //! Returns the file descriptor for the socket of a client given a client id
    int get_client_fd(char&);

    //! Returns the id of a client given a file descriptor
    char get_client_id(int&);

    //! Returns the client_info struct of a client given a client id
    client_info& get_client(char&);

    //! Returns the client_info struct of a client given a file descriptor
    client_info& get_client(int&);

private:
    char* ip_addr;
    char* port;
    client_info* clients;
    int count;
    int fd;
    bool done;
};

#endif
