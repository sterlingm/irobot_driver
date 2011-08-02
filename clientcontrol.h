#ifndef CLIENTCONTROL_H
#define CLIENTCONTROL_H
#include "tcpclient.h"

class ClientControl {
public:

    //!A Constructor
    ClientControl();
    //!A Desctructor
    ~ClientControl();

    //! Getter function for myClient member
    /*! Returns a reference to myClient member */
    TcpClient*& getClient();

    //! Setter function for myClient member
    /*! Sets myClient to c */
    void setClient(TcpClient*);


    //! Controls the client
    /*!
     * Controls the client\n
     * Creates the two threads for the client\n
     * Calls myClient's communicate function
     */
    void control();


private:

    static void* update_server_thread(void*);
    void update_server_thread_i();

    static void* driving_thread(void*);
    void driving_thread_i();

    TcpClient* myClient;
};
#endif
