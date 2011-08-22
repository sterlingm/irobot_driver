#include "tcpserver.h"
#include "agent.h"
#include <netinet/tcp.h>


TcpServer::TcpServer(char* p) : port(p), done(false) {}

TcpServer::~TcpServer() {}

/*Getter and setter for myAgent*/
void TcpServer::setAgent(Agent* a) {myAgent = a;}
Agent*& TcpServer::getAgent() {return myAgent;}

/*Getter and setter for ip_addr*/
void TcpServer::setIP(char* addr) {ip_addr = addr;}
char* TcpServer::getIP() {return ip_addr;}


/*Waits to connect a client. Returns true if successful*/
bool TcpServer::launchServer() {
    int status;

    struct addrinfo hints;
    struct addrinfo *servinfo;  //will point to the results

    //store the connecting address and size
    struct sockaddr_storage their_addr;
    socklen_t their_addr_size;


    memset(&hints, 0, sizeof hints); //make sure the struct is empty
    hints.ai_family = AF_INET;  //ipv4
    hints.ai_socktype = SOCK_STREAM; //tcp

    //get server info, put into servinfo
    if ((status = getaddrinfo((const char*)ip_addr, port, &hints, &servinfo)) != 0) {
        printf("\ngetaddrinfo error: %m", errno);
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
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(char*) &yes,sizeof(int)) == -1) {
        perror("setsockopt");
        return false;
    }

    //bind
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
    //std::cout<<"\ntempCommand: "<<tempCommand;

    //count the number of headers
    int num_headers = 0;
    for(int i=0;i<tempCommand.length() && num_headers < 2;i++)
        if(tempCommand[i] == '@')
            num_headers++;

    //if no message stacking
    if(num_headers == 1) {


        //if packet id 1 (update pos/goal/sensorvalues)
        if(tempCommand[2] == '1') {

            //get position row
            int prowend = 4;
            while(isdigit(tempCommand[prowend]))
                prowend++;
            std::string p = tempCommand.substr(4, prowend-3);
            int prow = atoi(p.c_str());
            //std::cout<<"\nprow: "<<prow;

            //get position column
            int pcolend = prowend+1;
            while(isdigit(tempCommand[pcolend]))
                pcolend++;
            std::string c = tempCommand.substr(prowend, pcolend-prowend);
            int pcol = atoi(c.c_str());
            //std::cout<<"\npcol: "<<pcol;

            //get goal row
            int growend = pcolend+1;
            while(isdigit(tempCommand[growend]))
                growend++;
            std::string gr = tempCommand.substr(pcolend, growend-pcolend);
            int grow = atoi(gr.c_str());
            //std::cout<<"\ngrow: "<<grow;

            //get goal column
            int gcolend = growend+1;
            while(isdigit(tempCommand[gcolend]))
                gcolend++;
            std::string gc = tempCommand.substr(growend, gcolend-growend);
            int gcol = atoi(gc.c_str());
            //std::cout<<"\ngcol: "<<gcol;


            //lock
            pthread_mutex_lock(&mutex_agent);

            //update
            myAgent->getPosition().setRow(prow);
            myAgent->getPosition().setCol(pcol);
            myAgent->getGoal().setRow(grow);
            myAgent->getGoal().setCol(gcol);


            //unlock
            pthread_mutex_unlock(&mutex_agent);

        }   //end if new goal


        //else if packet id 2 (change sensor)
        else if(tempCommand[2] == '2') {
            int idend = 4;
            while(isdigit(tempCommand[idend]))
                idend++;
            std::string temp = tempCommand.substr(4, idend-2);
            std::cout<<"\ntemp: "<<temp;
            int id = atoi(temp.c_str());

            //if valid id value, set new current sensor
            if(id > 6 && id < 43)
                myAgent->getRobot()->setCurrentSensor(id);

            else
                std::cout<<"\n"<<id<<" is Invalid id";

        }   //end if new sensor

        //else if 5, quit
        else if(tempCommand[2] == '5')
            done = true;

    }   //end if 1 header
    //else if stacking
    else
        std::cout<<"\nSTACKED MESSAGE: "<<tempCommand;

}   //END GETSENDBACK



/*Sends the updated path to the client*/
void TcpServer::sendPath(Path& p) {

    //message to send
    std::stringstream tosend;
    //add header and id and number of positions in the path
    tosend<<"@ 6 "<<p.getSize()<<" ";

    //get the list of positions in separate stringstream
    std::stringstream list_of_pos;
    for(int i=0;i<myAgent->getPath().getSize()-1;i++)
        list_of_pos<<myAgent->getPath().getPath().at(i).toString()<<" ";
    //add last position with no space on the end
    list_of_pos<<myAgent->getPath().getPath().back().toString();

    //std::cout<<"\nlist_of_pos: "<<list_of_pos.str();

    //concatenate the list
    tosend<<list_of_pos.str();
    //std::cout<<"\ntosend: "<<tosend.str();

    //send
    int num_read = send(comm_fd, tosend.str().c_str(), tosend.str().length(), 0);
}



/*Non-blocking communication with client*/
void TcpServer::communicate() {


    fd_set read_flags,write_flags; // the flag sets to be used
    struct timeval waitd = {10, 0};          // the max wait time for an event
    int sel;        // holds return value for select();
<<<<<<< HEAD
    int numRead = 0;    //holds return value for read()
    int numSent = 0;    //holds return value for send()
    char in[255];   //in buffer
    char out[512];  //out buffer
=======
    int numRead;    //holds return value for read()
    int numSent;    //holds return value for send()
    char in[255];   //in buffer
    char out[255];  //out buffer
>>>>>>> ea404a9f386303bbadd3b207982390732e17ae36

    //clear buffersz
    memset(&in, 0, 255);
    memset(&out, 0, 512);


    while(!done) {
        FD_ZERO(&read_flags);
        FD_ZERO(&write_flags);
        FD_SET(comm_fd, &read_flags);
        FD_SET(comm_fd, &write_flags);
        FD_SET(STDIN_FILENO, &read_flags);
        FD_SET(STDIN_FILENO, &write_flags);

        //call select
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
                //std::cout<<"\nClient: "<<in;
                getSendBack(in);
            }   //end if message
        }   //end if ready for read


        //if stdin is ready for reading
        if(FD_ISSET(STDIN_FILENO, &read_flags))
            fgets(out, 512, stdin);


        //if socket ready for writing
        if(FD_ISSET(comm_fd, &write_flags)) {

            //printf("\nSocket ready for write");
            FD_CLR(comm_fd, &write_flags);

            //check validity by checking for a digit
            if(isdigit(out[0])) {

                //create message to send
                std::stringstream tosend;
                tosend<<"@ "<<out;
                //std::cout<<"\ntosend: "<<tosend.str();

                //send
                numSent = send(comm_fd, tosend.str().c_str(), tosend.str().length(), 0);
            }   //end if valid message
            //if error, exit
            if(numSent < 0) {
                printf("\nError sending %i %m", comm_fd, errno);
                done = true;
            }   //end if error
            //wait for message to get there, then clear
            usleep(5000);
            memset(&out, 0, 512);
        }   //end if
    }   //end while
}   //END COMMUNICATE
