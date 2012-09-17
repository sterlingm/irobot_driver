#ifndef OWNCONTROL_H
#define OWNCONTROL_H
#include "robot_driver_agent.h"

class owncontrol {

public:

    //! Constructor\nTakes in an agent pointer for myAgent member
    owncontrol(Agent*);

    //! Creates threads and begins communication for user input
    void control();

private:

    Agent* myAgent;

    pthread_t display;
    pthread_t update_path;
    pthread_t get_sensors;
    pthread_t drive;

    bool done;

    static void* update_path_thread(void*);
    void update_path_thread_i();

    static void* display_menu_thread(void*);
    void display_menu_thread_i();

    static void* get_sensors_thread(void*);
    void get_sensors_thread_i();

    static void* drive_thread(void*);
    void drive_thread_i();

    void get_input();
};
#endif
