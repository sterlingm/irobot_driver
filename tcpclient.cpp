
#include "tcpclient.h"
#include "agent.h"
#include <netinet/tcp.h>

TcpClient::TcpClient(char* p) : port(p), done(false), update_agent(new char[6]), myAgent(NULL) {}

TcpClient::~TcpClient() {}

/*Getter for update_agent*/
char*& TcpClient::get_agent_update() {return update_agent;}

/*Getter and setter for myAgent*/
void TcpClient::setAgent(Agent* a) {myAgent = a;}
Agent*& TcpClient::getAgent() {return myAgent;}


/*Tries to connect to a server. Returns true if successful*/
bool TcpClient::launchClient() {
    int status, sock, adrlen;

    struct addrinfo hints;
    struct addrinfo *servinfo;  //will point to the results

    memset(&hints, 0, sizeof hints); //make sure the struct is empty
    hints.ai_family = AF_INET;
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

    if(connect(fd, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
        printf("\nclient connection failure %m", errno);
        return false;
    }

    return true;
}   //END LAUNCHCLIENT


/*Stores current position and goal and sends information to server*/
void TcpClient::updateServerAgent() {

    std::stringstream messagelength;
    messagelength<<"1 "<<myAgent->getPosition().getRow()<<" "<<myAgent->getPosition().getCol()<<" "<<myAgent->getGoal().getRow()<<" "<<myAgent->getGoal().getCol();


    std::string tempStrLen = messagelength.str();



    int prowend = 3;
    while(isdigit(tempStrLen[prowend]))
        prowend++;


    int pcolend = prowend+1;
    while(isdigit(tempStrLen[pcolend]))
        pcolend++;


    int growend = pcolend+1;
    while(isdigit(tempStrLen[growend]))
        growend++;


    int gcolend = growend+1;
    while(isdigit(tempStrLen[gcolend]))
        gcolend++;


    int length_of_rest = gcolend;
    std::cout<<"\ntempStrLen: "<<tempStrLen;
    std::cout<<"\nlength: "<<length_of_rest;

    //hold the whole message to be sent back
    std::stringstream temp;
    temp<<"- "<<length_of_rest<<" "<<messagelength.str();
    std::cout<<"\ntemp: "<<temp.str();

    std::string message = temp.str();
    std::cout<<"\nmessage: "<<message;


    //get the number of digits in length_of_rest
    int length_digits=0;
    while(isdigit(message[length_digits+2]))
        length_digits++;
    std::cout<<"\nlength_digits: "<<length_digits;

    //make message to send back. +1 length for \0
    char* back = new char[message.length()+1];

    for(int i=0;i<message.length();i++)
        back[i] = message[i];
    back[message.length()] = '\0';

    std::cout<<"\nback: "<<back;


    int numSent = send(fd, back, message.length()+1, 0);

}   //END UPDATESERVERAGENT


/*Processes a command from the server*/
void TcpClient::getCommand(char* command) {
    int numSent;
    std::string tempCommand = command;

    std::cout<<"\ncommand: "<<tempCommand;

    //id 1, change goal
    if(command[0] == '1') {

        //get row from command
        int rowend = 3;
        while(isdigit(command[rowend]))
            rowend++;
        int row = atoi(tempCommand.substr(2, rowend-1).c_str());



        //get column from command
        int colstart = rowend+1;
        int colend = colstart+1;
        while(isdigit(command[colend]))
            colend++;
        int col = atoi(tempCommand.substr(colstart, colend-colstart).c_str());

        //create temp position
        Position temp(row, col);


        //std::cout<<"\nNEW GOAL VALUE: "<<myAgent->getGrid()->getPos(row, col);
        //std::cout<<"\nrow: "<<row<<" col: "<<col<<"\n";
        //std::cout<<"\ntemp: "<<temp.toString()<<"\n";

        //lock
        pthread_mutex_lock(&UTILITY_H::mutex_agent_goal);
        pthread_mutex_lock(&UTILITY_H::mutex_agent_path);
        pthread_mutex_lock(&UTILITY_H::mutex_agent_pos);

        //set new goal
        myAgent->setGoal(temp);
        //traverse
        Path new_path = myAgent->traverse(myAgent->getGoal());
        //add duplicate for bug
        new_path.getPath().insert(new_path.getPath().begin(), myAgent->getPosition());
        //set new path
        myAgent->setPath(new_path);

        //unlock
        pthread_mutex_unlock(&UTILITY_H::mutex_agent_goal);
        pthread_mutex_unlock(&UTILITY_H::mutex_agent_path);
        pthread_mutex_unlock(&UTILITY_H::mutex_agent_pos);


    }   //end if 1


    //id 2, change sensor
    else if(command[0] == '2') {

        //isolate id
        int idend = 3;
        while(isdigit(command[idend]))
            idend++;
        std::string temp = tempCommand.substr(2, idend-2);
        int id = atoi(temp.c_str());

        if(id > 6 && id < 43)
            myAgent->getRobot()->setCurrentSensor(id);
        else
            std::cout<<"\n"<<id<<" is Invalid id";

        //length of '2 id'
        int l = 2 + temp.length();
        //- l ...
        int length = 3 + l;


        //hold message
        std::stringstream message;
        message<<"- "<<l<<" ";
        message<<command;
        std::cout<<"\nSENSOR message: "<<message.str();

        char back[message.str().length()+1];


        for(int i=0;i<message.str().length();i++)
            back[i] = message.str()[i];
        back[message.str().length()] = '\0';

        std::cout<<"\nback: "<<back;

        numSent = send(fd, back, message.str().length()+1, 0);
    }   //end if 2


    //id 3, toggle sensor streaming
    else if(command[0] == '3')
        myAgent->getRobot()->toggleSensorStream();
    //id 4, turn 360
    else if(command[0] == '4')
        myAgent->getRobot()->turnXDegrees(360);

    //id 5, quit
    else if(command[0] == '5') {
        char back[6];
        back[0] = '-';
        back[1] = ' ';
        back[2] = '1';
        back[3] = ' ';
        back[4] = '5';
        back[5] = '\0';

        numSent = send(fd, back, 6, 0);
        done = true;
    }   //end if 5
}   //END GETCOMMAND


/*Non-blocking communication with server*/
void TcpClient::communicate() {

    fd_set read_flags,write_flags; // the flag sets to be used
    struct timeval waitd = {10, 0};          // the max wait time for an event
    int sel;                      // holds return value for select();

    int numSent;
    int numRead;
    char in[255];
    char out[255];
    memset(&in, 0, 255);
    memset(&out, 0, 255);


    while(!done) {

        FD_ZERO(&read_flags);
        FD_ZERO(&write_flags);
        FD_SET(fd, &read_flags);
        FD_SET(fd, &write_flags);
        FD_SET(STDIN_FILENO, &read_flags);
        FD_SET(STDIN_FILENO, &write_flags);


        sel = select(fd+1, &read_flags, &write_flags, (fd_set*)0, &waitd);


        if(sel < 0)
            continue;


        //if socket ready for reading
        if(FD_ISSET(fd, &read_flags)) {

            //clear set
            FD_CLR(fd, &read_flags);

            //clear in
            memset(&in, 0, 255);

            numRead = recv(fd, in, 255, 0);
            //if an error, exit
            if(numRead < 0 && done) {
                printf("\nError reading %m", errno);
                done = true;
            }   //end if error
            //if connection closed, exit
            else if(numRead == 0) {
                printf("\nClosing socket");
                close(fd);
                done = true;
            }   //end if connection closed
            //if a message, call get command
            else if(in[0] != '\0') {
                //cout<<"\nServer: "<<in;
                getCommand(in);
                memset(&in, 0, 255);
            }   //end if message
        }   //end if ready for read


        //if stdin is ready for reading
        if(FD_ISSET(STDIN_FILENO, &read_flags))
            fgets(out, 255, stdin);


        //if socket ready for writing
        if(FD_ISSET(fd, &write_flags)) {
            //clear fd
            FD_CLR(fd, &write_flags);


            //check for message corruptness!!
            int numHeaders = 0;
            std::stringstream temp;
            temp<<out;
            for(int i=0;i<temp.str().length();i++)
                if(temp.str()[i] == '-')
                    numHeaders++;


            if(numHeaders == 1) {
                //send
                numSent = send(fd, out, 255, 0);
                //if an error, exit
                if(numSent < 0) {
                    printf("\nError sending %m", errno);
                    done = true;
                }   //end if an error
                //clear
            }
            memset(&out, 0, 255);

        }   //end if
    }   //end while

    cout<<"\n\nExiting normally\n";
}   //END COMMUNICATE
