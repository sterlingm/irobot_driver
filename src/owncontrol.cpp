#include "owncontrol.h"
#include <stdlib.h>
#include <errno.h>


owncontrol::owncontrol(Agent* a) : myAgent(a), done(false) {}


/*Callback function for update_path thread*/
void* owncontrol::update_path_thread(void* threadid) {
    owncontrol* f = (owncontrol*)threadid;
    f->update_path_thread_i();
}   //END UPDATE_PATH_THREAD


/*Inline function for update_path. Infinitely updates the agent's path every 0.2 seconds*/
inline void owncontrol::update_path_thread_i() {

    //while the server is running
    while(!done) {

        if(myAgent->get_algorithm() == ASTAR)
            //sleep for 0.2 seconds
            usleep(ASTAR_UPDATE_PATH_TIME);
        else if(myAgent->get_algorithm() == RRT)
            //sleep for 0.2 seconds
            usleep(RRT_UPDATE_PATH_TIME);

        //lock
        pthread_mutex_lock(&mutex_agent);

        //clear grid
        myAgent->getGrid()->clear();
        //try to find a path
        try {
            //set newPath to the path traverse returns
            Path newPath;
            if(myAgent->get_algorithm() == ASTAR)
                newPath = myAgent->getGridAnalyzer()->astar_path(myAgent->getPosition(), myAgent->getGoal());
            else if(myAgent->get_algorithm() == RRT)
                newPath = myAgent->getGridAnalyzer()->rrt_path(myAgent->getPosition(), myAgent->getGoal());


            //insert duplicate front position for bug
            //newPath.insert(myAgent->getPosition(), 0);

            //set new path
            myAgent->setPath(newPath);

        } catch(Agent::NoPathException* e) {}

        //lock
        pthread_mutex_unlock(&mutex_agent);
    }   //end while
}   //END UPDATE_PATH_THREAD_I


/*Callback function for display thread*/
void* owncontrol::display_menu_thread(void* threadid) {
    owncontrol* s = (owncontrol*)threadid;
    s->display_menu_thread_i();
}   //END DISPLAY_MENU_THREAD


/*Inline function for display thread. Infinitely displays the menu every 0.85 seconds*/
inline void owncontrol::display_menu_thread_i() {
    while(!done) {
        //usleep for x time
        usleep(1000000);

        //clear the screen
        system("clear");

        //lock
        pthread_mutex_lock(&mutex_agent);

        char display = myAgent->getRobot()->getID();

        //mark chosen robot's path
        myAgent->getGrid()->markPath(myAgent->getPath(), display);

        //make local variables to display
        Grid* g = myAgent->getGrid();
        Path p = myAgent->getPath();
        if(p.getSize() == 1)
            p.insert(p.getPathVector()[0], 0);
        int current_s = myAgent->getCurrentSensor();
        int highsv = myAgent->getHighSV();
        int lowsv = myAgent->getLowSV();

        std::string direction;

        switch (myAgent->getDirection()) {
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
        std::cout<<"\n"<<g->toString();
        std::cout<<"\nRobot "<<display<<" Information";
        std::cout<<"\nPath: "<<p.toString();
        std::cout<<"\nDirection: "<<direction;
        std::cout<<"\nSensor Packet "<<current_s<<" high byte: "<<highsv<<" low byte: "<<lowsv;
        std::cout<<"\n\nChange goal - g ROW COLUMN";
        std::cout<<"\nChange sensor packet - s PACKET-ID";
        std::cout<<"\nToggle sensor streaming - t";
        std::cout<<"\nSpin for sensors - p";
        std::cout<<"\nQuit - q\n";
        //unlock
        pthread_mutex_unlock(&mutex_agent);
    }   //end infinite for
}   //END DISPLAY_MENU_THREAD_I



/*Callback function for get_sensor thread*/
void* owncontrol::get_sensors_thread(void* threadid) {
    owncontrol* s = (owncontrol*)threadid;
    s->get_sensors_thread_i();
}   //END GET_SENSOR_THREAD


/*Inline function for get_sensor. Infinitely grabs the specified sensor every 15 milliseconds*/
inline void owncontrol::get_sensors_thread_i() {
    while(!done) {

        //sleep for 15ms
        usleep(15000);

        //grab sensor values
        sensor_packet temp = myAgent->getRobot()->get_sensor_value(myAgent->getCurrentSensor());
        //std::cout<<"\nsensor high byte value: "<<temp.values[1]<<"\n";
        //std::cout<<"\nsensor low byte value: "<<temp.values[0]<<"\n";

        myAgent->setHighSV(temp.values[1]);
        myAgent->setLowSV(temp.values[0]);
    }   //end while
}   //END GET_SENSOR_THREAD_I


/*Callback for drive thread*/
void* owncontrol::drive_thread(void* threadid) {
    owncontrol* c = (owncontrol*)threadid;
    c->drive_thread_i();
}   //END DRIVING_THREAD


/*Inline for drive thread. Makes the robot drive infinitely*/
inline void owncontrol::drive_thread_i() {
    //usleep(1000000);
    while(!done)
        myAgent->stepPath(false);
}   //END DRIVING_THREAD_I


void owncontrol::get_input() {

    std::string input;
    while(!done) {
        getline(std::cin, input);

        if(input[0] == 'g') {

                //get row from input
                int rowend = 2;
                while(isdigit(input[rowend]))
                    rowend++;
                //std::cout<<"\nrowend:"<<rowend<<"\n";
                //std::cout<<"\ninput:"<<input<<"\n";
                std::string row_str(input.substr(2, rowend-2).c_str());
                //std::cout<<"\nrow_str: "<<row_str;
                int row = atoi(row_str.c_str());

                //get column from input
                int colstart = rowend+1;
                int colend = colstart+1;
                while(isdigit(input[colend]) || input[colend] == '-')
                    colend++;
                std::string col_str(input.substr(colstart, colend-colstart));
                //std::cout<<"\ncol_str: "<<col_str;
                int col = atoi(col_str.c_str());

                //create temp position
                Position temp(row, col);
                //std::cout<<"\ntemp: "<<temp.toString();


                //std::cout<<"\nNEW GOAL VALUE: "<<myAgent->getGrid()->getPos(row, col);
                //std::cout<<"\nrow: "<<row<<" col: "<<col<<"\n";

                //check pointer
                if(myAgent)
                    //set new goal
                    myAgent->setGoal(temp);
                    //std::cout<<"\nnew goal is now "<<myAgent->getGoal().toString();
        }   //end if goal change

        //id s, change sensor
        else if(input[0] == 's') {

            //isolate id
            int idend = 2;
            while(isdigit(input[idend]))
                idend++;
            std::string temp(input.substr(2, idend-2));
            //std::cout<<"\ntemp: "<<temp;
            int id = atoi(temp.c_str());

            //check id
            //if invalid, just print and close function
            if(id < 7 || id > 42)
                std::cout<<"\n"<<id<<" is Invalid id";

            //else if valid, set new sensor and send message back to server
            else {

                //check pointer
                if(myAgent)
                    myAgent->setCurrentSensor(id);
            }   //end if valid sensor
        }   //end if s

        //id p, turn 360
        else if(input[0] == 'p' && myAgent) {
            //wait to finish driving
            while(myAgent->is_driving()) {}
            myAgent->set_spinning(true);
            myAgent->getRobot()->turnXDegrees(360);
            myAgent->set_spinning(false);
        }

        //id t, toggle sensor streaming
        else if(input[0] == 't' && myAgent)
            myAgent->getRobot()->toggleSensorStream();

                //id q, quit
        else if(input[0] == 'q')
            done = true;
        usleep(1000);
        input = "";
    }
}


void owncontrol::control() {

    std::cout<<"\nin here";
    std::cout<<"\ngrid:\n"<<myAgent->getGrid()->toString();

    //create threads
    pthread_create(&get_sensors, 0, get_sensors_thread, (void*)this);
    pthread_create(&update_path, 0, update_path_thread, (void*)this);
    pthread_create(&display, 0, display_menu_thread, (void*)this);
    pthread_create(&drive, 0, drive_thread, (void*)this);

    get_input();

    //detach all threads
    if(pthread_detach(update_path) != 0)
        printf("\ndetach on updating path for client 1 failed with error %m", errno);
    if(pthread_detach(get_sensors) != 0)
        printf("\ndetach on udp communication threat failed with error %m", errno);
    if(pthread_detach(drive) != 0)
        printf("\ndetach on display thread failed with error %m", errno);
    if(pthread_detach(display) != 0)
        printf("\ndetach on display thread failed with error %m", errno);
}   //END CONTROL

