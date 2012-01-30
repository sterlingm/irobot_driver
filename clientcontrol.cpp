#include "clientcontrol.h"
#include "robot_driver_agent.h"
#include <pthread.h>


pthread_t drive;
pthread_t update;
pthread_t c_udp_comm;



ClientControl::ClientControl(TcpClient* tc, Udpclient* uc) : myClient(tc), myUDP(uc) {}
ClientControl::~ClientControl() {}

/*Getter and setter for myClient*/
TcpClient*& ClientControl::getClient() {return myClient;}
void ClientControl::setClient(TcpClient* c) {myClient = c;}

/*Getter and setter for myUDP*/
Udpclient*& ClientControl::getUDP() {return myUDP;}
void ClientControl::setUDP(Udpclient* uc) {myUDP = uc;}


/*Callback for drive thread*/
void* ClientControl::driving_thread(void* threadid) {
    ClientControl* c = (ClientControl*)threadid;
    c->driving_thread_i();
}   //END DRIVING_THREAD


/*Inline for drive thread. Makes the robot drive infinitely*/
inline void ClientControl::driving_thread_i() {
    //usleep(1000000);
    while(!myClient->getDone())
        myClient->getAgent()->stepPath(false);
}   //END DRIVING_THREAD_I


/*Callback for update thread*/
void* ClientControl::update_server_thread(void* threadid) {
    ClientControl* c = (ClientControl*)threadid;
    c->update_server_thread_i();
}   //END UPDATE_SERVER_THREAD


/*Inline for update thread. Updates the server every second*/
inline void ClientControl::update_server_thread_i() {
    while(!myClient->getDone()) {
        usleep(UPDATE_SERVER_TIME);
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
    pthread_create(&c_udp_comm, 0, udp_comm_thread, (void*)this);
    pthread_create(&drive, 0, driving_thread, (void*)this);
    pthread_create(&update, 0, update_server_thread, (void*)this);

    //communicate
    myClient->communicate();

    //detach threads
    if(pthread_detach(c_udp_comm) != 0)
        printf("\ndetach on udp communication thread failed with error %m", errno);
    if(pthread_detach(drive) != 0)
        printf("\ndetach on drive thread failed with error %m", errno);
    if(pthread_detach(update) != 0)
        printf("\ndetach on update server failed with error %m", errno);

}   //END CONTROL
