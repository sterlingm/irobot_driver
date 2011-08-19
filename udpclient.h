#ifndef UDPCLIENT_H
#define UDPCLIENT_H


class Agent;

class udpclient {
public:

    //! Constructor
    /*! Constructor\nSets port to p*/
    udpclient(char*);
    //! Destructor
    /*! Destructor */
    ~udpclient();

    //! Setter function for myAgent member
    /*! Sets myAgent to a */
    void setAgent(Agent*);

    //! Getter function for myAgent member
    /*! Returns a reference to myAgent member*/
    Agent*& getAgent();

    //! Setter function for ip_addr member
    /*! Sets ip_addr to addr*/
    void setIP(char*);

    //! Getter function for ip_addr member
    /*! Returns the value of ip_addr*/
    char* getIP();

    //! Launches udp client
    /*!
     * Tries to create a udp client\n
     * Returns true on success and false otherwise
     */
    bool launch_client();

    //! Sends messages to the server\n
    /*! The client sends sensor information to the server every 15 milliseconds */
    void communicate();

private:
    int fd;
    char* ip_addr;
    char* port;
    Agent* myAgent;
};

#endif
