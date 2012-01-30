#include "servercontrol.h"

//struct to pass for updating path threads
typedef struct {
    ServerControl* sc;
    char id;
} myStruct;

ServerControl::ServerControl(TcpServer* ts, Udpserver* us) : myServer(ts), myUDP(us), update_path(new pthread_t[ts->get_num_clients()]) {}
ServerControl::~ServerControl() {delete [] update_path; update_path = 0;}


/*Getter and setter for myServer*/
TcpServer*& ServerControl::getServer() {return myServer;}
void ServerControl::setServer(TcpServer* s) {myServer = s;}

/*Getter and setter for myUDP*/
void ServerControl::setUDP(Udpserver* us) {myUDP = us;}
Udpserver*& ServerControl::getUDP() {return myUDP;}

/*Callback function for update_path thread*/
void* ServerControl::update_path_thread(void* threadid) {
    myStruct* f = (myStruct*)threadid;
    f->sc->update_path_thread_i(f->id);
}   //END UPDATE_PATH_THREAD


/*Inline function for update_path. Infinitely updates the agent's path every 0.2 seconds*/
inline void ServerControl::update_path_thread_i(char client_id) {
    //while the server is running
    while(!myServer->getDone()) {

        //sleep for 0.2 seconds
        usleep(UPDATE_PATH_TIME);


        pthread_mutex_lock(&mutex_agent);

        //clear grid and mark all other robots
        myServer->get_client(client_id).agent->getGrid()->clear();
        for(int i=0;i<myServer->get_num_clients();i++) {
            //if not this client
            if(myServer->get_clients()[i].id != client_id) {
                Agent* temp_agent = myServer->get_clients()[i].agent;
                myServer->get_client(client_id).agent->getGrid()->setPos(temp_agent->getPosition().getRow(),
                                                                         temp_agent->getPosition().getCol(),
                                                                         myServer->get_clients()[i].id);
            }   //end if
        }   //end for
        //std::cout<<"\nGRID BEFORE TRAVERSE FOR CLIENT"<<client_id<<": \n"<<myServer->get_client(client_id).agent->getGrid()->toString();

        //try to find a path
        try {
            //set newPath to the path traverse returns
            Path newPath = myServer->get_client(client_id).agent->astar_path(myServer->get_client(client_id).agent->getGoal());

            //insert duplicate front position for bug
            newPath.insert(myServer->get_client(client_id).agent->getPosition(), 0);

            //set new path
            myServer->get_client(client_id).agent->setPath(newPath);

            //send path
            myServer->sendPath(newPath, client_id);

        } catch(Agent::NoPathException* e) {}

        //unlock
        pthread_mutex_unlock(&mutex_agent);
    }   //end while
}   //END UPDATE_PATH_THREAD_I




/*Callback function for display thread*/
void* ServerControl::display_menu_thread(void* threadid) {
    ServerControl* s = (ServerControl*)threadid;
    s->display_menu_thread_i();
}   //END DISPLAY_MENU_THREAD


/*Inline function for display thread. Infinitely displays the menu every 0.85 seconds*/
inline void ServerControl::display_menu_thread_i() {
    while(!myServer->getDone()) {
        //usleep for x time
        usleep(MENU_SLEEP_TIME);

        //clear the screen
        system("clear");


        char display = myServer->getWhichDisplay();

        //lock
        pthread_mutex_lock(&mutex_agent);


        //mark chosen robot's path
        myServer->get_client(display).agent->getGrid()->markPath(myServer->get_client(display).agent->getPath(), display);
        //display the rest
        for(int i=0;i<myServer->get_num_clients();i++) {
            //if not this client
            if(myServer->get_clients()[i].id != myServer->getWhichDisplay()) {
                Agent* temp_agent = myServer->get_clients()[i].agent;
                myServer->get_clients()[i].agent->getGrid()->setPos(temp_agent->getPosition().getRow(),
                                                                    temp_agent->getPosition().getCol(),
                                                                        myServer->get_clients()[i].id);
            }   //end if
        }   //end for

        //make local variables to display
        Grid* g = myServer->get_client(display).agent->getGrid();
        Path p = myServer->get_client(display).agent->getPath();
        int velocity = myServer->get_client(display).agent->getRobot()->getVelocity();
        int current_s = myServer->get_client(display).agent->getCurrentSensor();
        int highsv = myServer->get_client(display).agent->getHighSV();
        int lowsv = myServer->get_client(display).agent->getLowSV();
        char mode = myServer->get_client(display).agent->get_mode();
        std::string direction;

        //get direction
        switch (myServer->get_client(display).agent->getDirection()) {
        case NORTH:
            direction = "n";
            break;
        case EAST:
            direction = "e";
            break;
        case SOUTH:
            direction = "s";
            break;
        case WEST:
            direction = "w";
            break;
        case NORTHEAST:
            direction = "ne";
            break;
        case NORTHWEST:
            direction = "nw";
            break;
        case SOUTHEAST:
            direction = "se";
            break;
        case SOUTHWEST:
            direction = "sw";
            break;
        }

        //print
        std::cout<<g->toString();
        std::cout<<"Robot "<<display<<" Information";
        std::cout<<"\nMode: "<<mode;
        std::cout<<"\nPath:"<<p.toString();
        std::cout<<"\nVelocity: "<<velocity;
        std::cout<<"\nDirection: "<<direction;
        std::cout<<"\nSensor Packet "<<current_s<<" high byte: "<<highsv<<" low byte: "<<lowsv;
        std::cout<<"\n\nChange Viewing Robot - r ROBOT-ID";
        std::cout<<"\nChange Goal - ROBOT-ID g ROW COLUMN";
        std::cout<<"\nChange Sensor Packet - ROBOT-ID s PACKET-ID";
        std::cout<<"\nChange Velocity - ROBOT-ID v VELOCITY";
        std::cout<<"\nChange Direction - ROBOT-ID d DIRECTION";
        std::cout<<"\nChange Mode - ROBOT-ID m f|s";
        std::cout<<"\nToggle Sensor Streaming - ROBOT-ID t";
        std::cout<<"\nSpin - ROBOT-ID p";
        std::cout<<"\nQuit - q\n";

        //unlock
        pthread_mutex_unlock(&mutex_agent);
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

    myStruct* structs = new myStruct[myServer->get_num_clients()];

    //set struct values
    for(int i=0;i<myServer->get_num_clients();i++) {
        structs[i].sc = this;
        structs[i].id = myServer->get_clients()[i].id;
    }   //end for

    //create threads
    pthread_create(&display, 0, display_menu_thread, (void*)this);
    pthread_create(&s_udp_comm, 0, udp_comm_thread, (void*)this);
    //make a thread for each client
    for(int i=0;i<myServer->get_num_clients();i++)
        pthread_create(&update_path[i], 0, update_path_thread, &structs[i]);

    //communicate with client
    myServer->communicate();

    //detach all threads
    for(int i=0;i<myServer->get_num_clients();i++)
        if(pthread_detach(update_path[i]) != 0)
            printf("\ndetach on updating path for client 1 failed with error %m", errno);
    if(pthread_detach(s_udp_comm) != 0)
        printf("\ndetach on udp communication thread failed with error %m", errno);
    if(pthread_detach(display) != 0)
        printf("\ndetach on display thread failed with error %m", errno);

    //cleanup
    delete [] structs;
    structs = 0;
}   //END CONTROL
