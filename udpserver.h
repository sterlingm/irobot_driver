#ifndef UDPSERVER_H
#define UDPSERVER_H


class Agent;

class udpserver {
public:

    //! Constructor
    /*! Constructor\nSets port to p*/
    udpserver(char*);
    //! Destructor
    /*! Destructor */
    ~udpserver();

    //! Setter function for myAgent member
    /*! Sets myAgent to a */
    void setAgent(Agent*);

    //! Getter function for myAgent member
    /*! Returns a reference to myAgent*/
    Agent*& getAgent();

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
    void get_message(char*);

    //! Receives messages from the client
    /*! Waits for the client to send a message */
    void communicate();

private:
    char* ip_addr;
    char* port;
    int fd;
    Agent* myAgent;
};

#endif
