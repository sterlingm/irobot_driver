#ifndef SERVERCONTROL_H
#define SERVERCONTROL_H
#include "tcpserver.h"
#include "udpserver.h"
#include "robot_driver_agent.h"

class ServerControl {
public:

	//! A Constructor
	ServerControl(TcpServer*, Udpserver*);
    //! A Destructor
    ~ServerControl();

	//! Getter function for myServer member
	/*! Returns reference to myServer member */
    TcpServer*& getServer();

	//! Setter function for myServer member
	/*! Sets myServer to s */
    void setServer(TcpServer*);

    //! Getter function for myUDP member
    /*! Returns a reference to myUDP member*/
    Udpserver*& getUDP();

    //! Setter function for myUDP member
    /*! Sets myUDP to us */
    void setUDP(Udpserver*);

	//! Controls the server
	/*!
	 * Controls the server\n
	 * Creates three threads for the client\n
	 * Calls myServer->communicate function
	 */
    void control();

private:

    TcpServer* myServer;
    Udpserver* myUDP;

    pthread_t display;
    pthread_t* update_path;
    pthread_t s_udp_comm;

    static void* update_path_thread(void*);
    void update_path_thread_i(char);

    static void* display_menu_thread(void*);
    void display_menu_thread_i();

    static void* udp_comm_thread(void*);
    void udp_comm_thread_i();

};
#endif
