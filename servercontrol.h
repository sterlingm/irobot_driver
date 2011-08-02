#ifndef SERVERCONTROL_H
#define SERVERCONTROL_H
#include "tcpserver.h"

class ServerControl {
public:

	//! A Constructor
	ServerControl();
    //! A Destructor
    ~ServerControl();

	//! Getter function for myServer member
	/*! Returns reference of myServer member */
    TcpServer*& getServer();

	//! Setter function for myServer member
	/*! Sets myServer to s */
    void setServer(TcpServer*);

	//! Controls the server
	/*!
	 * Controls the server\n
	 * Creates three threads for the client\n
	 * Calls myServer->communicate function
	 */
    void control();

private:

    TcpServer* myServer;

    static void* update_path_thread(void*);
    void update_path_thread_i();

    static void* display_menu_thread(void*);
    void display_menu_thread_i();

    static void* get_sensor_thread(void*);
    void get_sensor_thread_i();

};
#endif
