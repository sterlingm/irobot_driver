#include "clientcontrol.h"
#include "agent.h"
#include <pthread.h>


pthread_t drive;
pthread_t update;
pthread_t c_get_sensor;

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

/*Callback for get sensor thread*/
void* ClientControl::get_c_sensor_thread(void* threadid) {
    ClientControl* c = (ClientControl*)threadid;
    c->get_c_sensor_thread_i();
}   //END GET_C_SENSOR_THREAD

/*Inline for get sensor thread. Polls the robot for sensor values every 15ms*/
inline void ClientControl::get_c_sensor_thread_i() {
    //sleep 15ms and grab new sensor value
    while(1) {
        usleep(15000);
        Sensor_Packet temp;
        temp = myClient->getAgent()->getRobot()->getSensorValue(myClient->getAgent()->getRobot()->getCurrentSensor());
        UTILITY_H::lowsv = temp.values[0];
        UTILITY_H::highsv = temp.values[1];
    }   //end while
}   //END GET_C_SENSOR_THREAD_I


/*Controls the client. Creates two threads and calls communicate*/
void ClientControl::control() {
    //make threads
    pthread_create(&drive, NULL, driving_thread, (void*)this);
    pthread_create(&update, NULL, update_server_thread, (void*)this);
    pthread_create(&c_get_sensor, NULL, get_c_sensor_thread, (void*)this);
    //communicate
    myClient->communicate();

}   //END CONTROL
