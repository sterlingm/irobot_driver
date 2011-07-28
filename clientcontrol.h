#ifndef CLIENTCONTROL_H
#define CLIENTCONTROL_H
#include "tcpclient.h"

class ClientControl {
public:

    //! Getter function for myClient member
    /*!
     * Returns a reference to myClient member
     */
    TcpClient*& getClient();

    //! Setter function for myClient member
    /*!
     * Sets myClient to c
     */
    void setClient(TcpClient*);


    //! Controls the client
    /*!
     * Controls the client
     * Creates the two threads for the client
     * Calls myClient's communicate function
     */
    void control();


private:

    //! Thread callback for updating the server's agent
    /*!
     * Calls update_server_thread_i()
     */
    static void* update_server_thread(void*);
    //! Inline function for updating_server_thread
    /*!
     * Infinitely calls myClient->updateServerAgent function every second
     */
    void update_server_thread_i();

    //! Thread callback for driving the robot
    /*!
     * Calls driving_thread_i
     */
    static void* driving_thread(void*);
    //! Inline function for driving_thread
    /*!
     * Infinitely calls myClient->getAgent()->stepPath()
     */
    void driving_thread_i();

    TcpClient* myClient;
};
#endif
