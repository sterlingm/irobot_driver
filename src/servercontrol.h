#ifndef SERVERCONTROL_H
#define SERVERCONTROL_H
#include <sys/time.h>
#include "tcpserver.h"
#include "udpserver.h"
#include "robot_driver_agent.h"

class ServerControl {
public:

	//! A Constructor
	ServerControl(TcpServer*, UdpServer*);
	//! A Constructor
	ServerControl(TcpServer*, UdpServer*, int&);
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
    UdpServer*& getUDP();

    //! Setter function for myUDP member
    /*! Sets myUDP to us */
    void setUDP(UdpServer*);

	//! Controls the server
	/*!
	 * Controls the server\n
	 * Creates three threads for the client\n
	 * Calls myServer->communicate function
	 */
    void control();

private:

    TcpServer* myServer;
    UdpServer* myUDP;

    pthread_t display;
    pthread_t* update_path;
    pthread_t rrt_update;
    pthread_t s_udp_comm;


    static void* update_path_thread(void*);
    void update_path_thread_i(char);

    static void* rrt_update_thread(void*);
    void rrt_update_thread_i();

    static void* display_menu_thread(void*);
    void display_menu_thread_i();

    static void* udp_comm_thread(void*);
    void udp_comm_thread_i();

    Position localize(Agent, struct timeval);

    Position last_known;
    struct timeval last_timestamp;
};
#endif
