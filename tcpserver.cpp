#include "tcpserver.h"
#include "agent.h"
#include <netinet/tcp.h>
    int numSent;
    int numRead;

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

    std::string tempCommand = command;

    //if message starts with header
    if(command[0] == '-') {

        //check for no message stacking
        int numHeaders = 0;
        for(int i=0;i<tempCommand.length();i++)
            if(tempCommand[i] == '-')
                numHeaders++;

        //if no message stacking
        if(numHeaders == 1) {

            //get the number of digits in the length
            int length_digit = 0;
            while(isdigit(command[length_digit+2]))
                length_digit++;
            //std::cout<<"\nlength_digit: "<<length_digit;


            int index = 2;
            std::stringstream iso_length;
            for(int i=0;i<length_digit;i++)
                iso_length<<command[index+i];
            //std::cout<<"\niso_length: "<<iso_length.str();


            //get length as int value
            int length = atoi(iso_length.str().c_str());
            //std::cout<<"\nlength: "<<length;

            //get the actual message
            std::string rest = tempCommand.substr(3+length_digit, length);
            //std::cout<<"\nrest:"<<rest<<"-";

            //check for the correct length
            if( rest.length() == length ) {

                //if packet id 1 (update pos/goal)
                if(rest[0] == '1') {

                    int prowend = 3;
                    while(isdigit(rest[prowend]))
                        prowend++;
                    std::string p = rest.substr(2, prowend-2);
                    int prow = atoi(p.c_str());
                    //std::cout<<"\nprow: "<<prow;


                    int pcolend = prowend+1;
                    while(isdigit(rest[pcolend]))
                        pcolend++;
                    std::string c = rest.substr(prowend, pcolend-prowend);
                    int pcol = atoi(c.c_str());
                    //std::cout<<"\npcol: "<<pcol;


                    int growend = pcolend+1;
                    while(isdigit(rest[growend]))
                        growend++;
                    std::string gr = rest.substr(pcolend, growend-pcolend);
                    int grow = atoi(gr.c_str());
                    //std::cout<<"\ngrow: "<<grow;


                    int gcolend = growend+1;
                    while(isdigit(rest[gcolend]))
                        gcolend++;
                    std::string gc = rest.substr(growend, gcolend-growend);
                    int gcol = atoi(gc.c_str());
                    //std::cout<<"\ngcol: "<<gcol;


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


                //else if packet id 2 (change sensor)
                else if(rest[0] == '2') {

                    int idend = 3;
                    while(isdigit(rest[idend]))
                        idend++;
                    std::string temp = rest.substr(2, idend-2);
                    int id = atoi(temp.c_str());


                    if(id > 6 && id < 43)
                        myAgent->getRobot()->setCurrentSensor(id);

                    else
                        std::cout<<"\n"<<id<<" is Invalid id";


                }   //end if new sensor

                else if(rest[0] == '5')
                    done = true;


            }   //end if no stacking
            else {
                std::cout<<"\nINCORRECT LENGTH";
                std::cout<<"\nrest:"<<rest;
                std::cout<<"\nrest.length(): "<<rest.length();
                std::cout<<"\nlength: "<<length;
                std::cout<<"\nlength_digit: "<<length_digit;
            }
        }   //end if correct length
    }   //end if header
}   //END GETSENDBACK



/*Non-blocking communication with client*/
void TcpServer::communicate() {


    fd_set read_flags,write_flags; // the flag sets to be used
    struct timeval waitd = {10, 0};          // the max wait time for an event
    int sel;                      // holds return value for select();

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
