#ifndef SERVERCONTROL_H
#define SERVERCONTROL_H
#include "tcpserver.h"
#include "udpserver.h"

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


    void setUDP(udpserver*);

	//! Controls the server
	/*!
	 * Controls the server\n
	 * Creates three threads for the client\n
	 * Calls myServer->communicate function
	 */
    void control();

private:

    TcpServer* myServer;
    udpserver* myUDP;


    static void* update_path_thread(void*);
    void update_path_thread_i();

    static void* display_menu_thread(void*);
    void display_menu_thread_i();

    static void* udp_comm_thread(void*);
    void udp_comm_thread_i();

};
#endif
