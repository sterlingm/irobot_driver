#include "tcpserver.h"
#include "agent.h"

TcpServer::TcpServer(char* p) : port(p), done(false) {}

TcpServer::~TcpServer() {}

/*Getter and setter for myAgent*/
void TcpServer::setAgent(Agent* a) {myAgent = a;}
Agent*& TcpServer::getAgent() {return myAgent;}


/*Waits to connect a client. Returns true if successful*/
bool TcpServer::launchServer() {
    int status;

    struct addrinfo hints;
    struct addrinfo *servinfo;  //will point to the results

    //store the connecting address and size
    struct sockaddr_storage their_addr;
    socklen_t their_addr_size;


    memset(&hints, 0, sizeof hints); //make sure the struct is empty
    hints.ai_family = AF_INET;  //local address
    hints.ai_socktype = SOCK_STREAM; //tcp
    hints.ai_flags = AI_PASSIVE;     //use local-host address

    //get server info, put into servinfo
    if ((status = getaddrinfo("127.0.0.1", port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return false;
    }

    //make socket
    fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (fd < 0) {
        printf("\nserver socket failure %m", errno);
        return false;
    }

    //allow reuse of port
    int yes=1;
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        return false;
    }

    //unlink and bind
    unlink("127.0.0.1");
    if(bind (fd, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
        printf("\nBind error %m", errno);
        return false;
    }

    //free up space
    freeaddrinfo(servinfo);

    //listen
    if(listen(fd, 5) < 0) {
        printf("\nListen error %m", errno);
        return false;
    }
    their_addr_size = sizeof(their_addr);


    //accept
    comm_fd = accept(fd, (struct sockaddr*)&their_addr, &their_addr_size);
    if( comm_fd < 0) {
        printf("\nAccept error %m", errno);
        return false;
    }

    return true;
}   //END LAUNCHSERVER



/*Gets a message sent back from client in response to a previously sent command*/
void TcpServer::getSendBack(char* command) {

    //if new goal
    if(command[0] == '1') {

        int prow = (int)command[1] - 48;
        int pcol = (int)command[2] - 48;
        int grow = (int)command[3] - 48;
        int gcol = (int)command[4] - 48;

        //lock
        pthread_mutex_lock(&UTILITY_H::mutex_agent_goal);
        pthread_mutex_lock(&UTILITY_H::mutex_agent_path);
        pthread_mutex_lock(&UTILITY_H::mutex_agent_pos);

        //update
        myAgent->getPosition().setRow(prow);
        myAgent->getPosition().setCol(pcol);
        myAgent->getGoal().setRow(grow);
        myAgent->getGoal().setCol(gcol);

        //unlock
        pthread_mutex_unlock(&UTILITY_H::mutex_agent_goal);
        pthread_mutex_unlock(&UTILITY_H::mutex_agent_path);
        pthread_mutex_unlock(&UTILITY_H::mutex_agent_pos);

    }   //end if new goal
    //if new sensor
    else if(command[0] == '2') {

        int id = (int)command[1] - 48;
        myAgent->getRobot()->setCurrentSensor(id);

    }   //end if sensor
    //if quit
    else if(command[0] == '5')
        done = true;
}   //END GETSENDBACK



/*Non-blocking communication with client*/
void TcpServer::communicate() {


    fd_set read_flags,write_flags; // the flag sets to be used
    struct timeval waitd = {10, 0};          // the max wait time for an event
    int sel;                      // holds return value for select();

    int numSent;
    int numRead;
    char in[255];   //in buffer
    char out[255];  //out buffer
    memset(&in, 0, 255);
    memset(&out, 0, 255);


    while(!done) {
        FD_ZERO(&read_flags);
        FD_ZERO(&write_flags);
        FD_SET(comm_fd, &read_flags);
        FD_SET(comm_fd, &write_flags);
        FD_SET(STDIN_FILENO, &read_flags);
        FD_SET(STDIN_FILENO, &write_flags);


        sel = select(comm_fd+1, &read_flags, &write_flags, (fd_set*)0, &waitd);

        //if an error with select
        if(sel < 0)
            continue;

        //if socket ready for reading
        if(FD_ISSET(comm_fd, &read_flags)) {

            //clear set
            FD_CLR(comm_fd, &read_flags);

            memset(&in, 0, 255);

            numRead = recv(comm_fd, in, 255, 0);
            //if an error, exit
            if(numRead < 0) {
                printf("\nError reading %m", errno);
                myAgent->getRobot()->pauseSensorStream();
                done = true;
            }   //end if error
            //if connection closed, exit
            else if(numRead == 0) {
                printf("\nClosing socket");
                close(comm_fd);
                done = true;
            }   //end if connection closed
            //if message, call getsendback
            else if(in[0] != '\0') {
                cout<<"\nClient: "<<in;
                getSendBack(in);
            }   //end if message
        }   //end if ready for read


        //if stdin is ready for reading
        if(FD_ISSET(STDIN_FILENO, &read_flags))
            fgets(out, 255, stdin);


        //if socket ready for writing
        if(FD_ISSET(comm_fd, &write_flags)) {

            //printf("\nSocket ready for write");
            FD_CLR(comm_fd, &write_flags);
            //send
            numSent = send(comm_fd, out, 255, 0);
            //if error, exit
            if(numSent < 0) {
                printf("\nError sending %m", errno);
                done = true;
            }   //end if error
            //clear
            memset(&out, 0, 255);
        }   //end if
    }   //end while
}   //END COMMUNICATE
