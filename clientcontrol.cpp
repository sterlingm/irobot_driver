#include "clientcontrol.h"
#include "agent.h"
#include <pthread.h>


pthread_t drive;
pthread_t update;
pthread_t c_udp_comm;

ClientControl::ClientControl() {}
ClientControl::~ClientControl() {}

/*Getter and setter for myClient*/
TcpClient*& ClientControl::getClient() {return myClient;}
void ClientControl::setClient(TcpClient* c) {myClient = c;}



void ClientControl::setUDP(udpclient* uc) {myUDP = uc;}


/*Callback for drive thread*/
void* ClientControl::driving_thread(void* threadid) {
    ClientControl* c = (ClientControl*)threadid;
    c->driving_thread_i();
}   //END DRIVING_THREAD


/*Inline for drive thread. Makes the robot drive infinitely*/
inline void ClientControl::driving_thread_i() {
    while(1)
        myClient->getAgent()->stepPath(false);
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

/*Callback for get sensor thread*/
void* ClientControl::udp_comm_thread(void* threadid) {
    ClientControl* c = (ClientControl*)threadid;
    c->udp_comm_thread_i();
}   //END GET_C_SENSOR_THREAD


/*Inline for get sensor thread. Polls the robot for sensor values every 15ms*/
inline void ClientControl::udp_comm_thread_i() {
    myUDP->communicate();
}   //END GET_C_SENSOR_THREAD_I


/*Controls the client. Creates two threads and calls communicate*/
void ClientControl::control() {
    //make threads
    pthread_create(&drive, NULL, driving_thread, (void*)this);
    pthread_create(&update, NULL, update_server_thread, (void*)this);
    pthread_create(&c_udp_comm, NULL, udp_comm_thread, (void*)this);
    //communicate
    myClient->communicate();

}   //END CONTROL
