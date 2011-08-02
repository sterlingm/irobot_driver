#include "clientcontrol.h"
#include "agent.h"
#include <pthread.h>


pthread_t drive;
pthread_t update;

ClientControl::ClientControl() {}
ClientControl::~ClientControl() {}

/*Getter and setter for myClient*/
TcpClient*& ClientControl::getClient() {return myClient;}
void ClientControl::setClient(TcpClient* c) {myClient = c;}


/*Callback for drive thread*/
void* ClientControl::driving_thread(void* threadid) {
    ClientControl* c = (ClientControl*)threadid;
    c->driving_thread_i();
}   //END DRIVING_THREAD


/*Inline for drive thread. Makes the robot drive infinitely*/
inline void ClientControl::driving_thread_i() {
    while(1)
        myClient->getAgent()->stepPath();
}   //END DRIVING_THREAD_I


/*Callback for update thread*/
void* ClientControl::update_server_thread(void* threadid) {
    ClientControl* c = (ClientControl*)threadid;
    c->update_server_thread_i();
}   //END UPDATE_SERVER_THREAD


/*Inline for update thread. Updates the server every second*/
inline void ClientControl::update_server_thread_i() {
    for(;;) {
        usleep(100000);
        myClient->updateServerAgent();
    }   //end while
}   //END UPDATE_AGENT_THREAD_I


/*Controls the client. Creates two threads and calls communicate*/
void ClientControl::control() {
    //make threads
    pthread_create(&drive, NULL, driving_thread, (void*)this);
    pthread_create(&update, NULL, update_server_thread, (void*)this);
    //communicate
    myClient->communicate();

}   //END CONTROL
