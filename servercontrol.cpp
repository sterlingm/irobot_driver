#include "servercontrol.h"
#include "agent.h"

pthread_t display;
pthread_t update_path;
pthread_t s_udp_comm;



ServerControl::ServerControl() {}
ServerControl::~ServerControl() {}


/*Getter and setter for myServer*/
TcpServer*& ServerControl::getServer() {return myServer;}
void ServerControl::setServer(TcpServer* s) {myServer = s;}


void ServerControl::setUDP(udpserver* us) {myUDP = us;}



/*Callback function for update_path thread*/
void* ServerControl::update_path_thread(void* threadid) {
    ServerControl* s = (ServerControl*)threadid;
    s->update_path_thread_i();
}   //END UPDATE_PATH_THREAD


/*Inline function for update_path. Infinitely updates the agent's path every 0.2 seconds*/
inline void ServerControl::update_path_thread_i() {
    for(;;) {
        //sleep for 0.2 seconds
        usleep(UTILITY_H::UPDATE_PATH_TIME);

        //lock goal
        pthread_mutex_lock(&UTILITY_H::mutex_agent);

        //std::cout<<"\nUpdating Path between "<<myServer->getAgent()->getPosition().toString()<<" to "<<myServer->getAgent()->getGoal().toString();
        //traverse
        myServer->getAgent()->getGrid()->clear();
        Path newPath = myServer->getAgent()->traverse(myServer->getAgent()->getGoal());
        //set new path
        newPath.getPath().insert(newPath.getPath().begin(), myServer->getAgent()->getPosition());
        myServer->getAgent()->setPath(newPath);

        //unlock
        pthread_mutex_unlock(&UTILITY_H::mutex_agent);

    }   //end infinite for
}   //END UPDATE_PATH_THREAD_I


/*Callback function for display thread*/
void* ServerControl::display_menu_thread(void* threadid) {
    ServerControl* s = (ServerControl*)threadid;
    s->display_menu_thread_i();
}   //END DISPLAY_MENU_THREAD


/*Inline function for display thread. Infinitely displays the menu every 0.85 seconds*/
inline void ServerControl::display_menu_thread_i() {
    for(;;) {
        //usleep for x time
        usleep(UTILITY_H::MENU_SLEEP_TIME);

        //lock
        pthread_mutex_lock(&UTILITY_H::mutex_agent);

        //clear the screen
        system("clear");

        //clear grid and mark the path
        myServer->getAgent()->getGrid()->clear();
        myServer->getAgent()->getGrid()->markPath(myServer->getAgent()->getPath());

        //unlock
        pthread_mutex_unlock(&UTILITY_H::mutex_agent);

        //print
        std::cout<<myServer->getAgent()->getGrid()->toString();
        std::cout<<"\nPath: "<<myServer->getAgent()->getPath().toString();
        std::cout<<"\nSENSOR PACKET "<<myServer->getAgent()->getRobot()->getCurrentSensor()<<": "<<myServer->getAgent()->getHighSV()<<"  "<<myServer->getAgent()->getLowSV();
        std::cout<<"\nChange goal - 1 ROW COLUMN";
        std::cout<<"\nChange sensor packet - 2 PACKET-ID";
        std::cout<<"\nToggle sensor streaming - 3";
        std::cout<<"\nSpin for sensors - 4";
        std::cout<<"\nQuit - 5\n";

    }   //end infinite for
}   //END DISPLAY_MENU_THREAD_I



/*Callback function for get_sensor thread*/
void* ServerControl::udp_comm_thread(void* threadid) {
    ServerControl* s = (ServerControl*)threadid;
    s->udp_comm_thread_i();
}   //END GET_SENSOR_THREAD


/*Inline function for get_sensor. Infinitely grabs the specified sensor every 15 milliseconds*/
inline void ServerControl::udp_comm_thread_i() {
    myUDP->communicate();
}   //END GET_SENSOR_THREAD_I


/*Controls the server. Creates three threads and calls communicate*/
void ServerControl::control() {

    //create threads
    pthread_create(&update_path, NULL, update_path_thread, (void*)this);
    pthread_create(&s_udp_comm, NULL, udp_comm_thread, (void*)this);
    pthread_create(&display, NULL, display_menu_thread, (void*)this);
    //communicate with client
    myServer->communicate();

}   //END CONTROL
