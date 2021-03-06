#ifndef CLIENTCONTROL_H
#define CLIENTCONTROL_H
#include "tcpclient.h"
#include "udpclient.h"

class ClientControl {
public:

    //!A Constructor
    ClientControl(TcpClient*, UdpClient*);
    //!A Desctructor
    ~ClientControl();

    //! Getter function for myClient member
    /*! Returns a reference to myClient member */
    TcpClient*& getClient();

    //! Setter function for myClient member
    /*! Sets myClient to c */
    void setClient(TcpClient*);

    //! Getter function for myUDP member
    /*! Returns a reference to myUDP member*/
    UdpClient*& getUDP();

    //! Setter function for myUDP member
    /*! Sets myUDP to uc*/
    void setUDP(UdpClient*);


    //! Controls the client
    /*!
     * Controls the client\n
     * Creates the two threads for the client\n
     * Calls myClient's communicate function
     */
    void control();


private:
    TcpClient* myClient;
    UdpClient* myUDP;

    static void* update_server_thread(void*);
    void update_server_thread_i();

    static void* driving_thread(void*);
    void driving_thread_i();

    static void* udp_comm_thread(void*);
    void udp_comm_thread_i();

};
#endif
