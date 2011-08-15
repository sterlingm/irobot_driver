
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
    int status, sock, adrlen;   //status, file descriptor, address length

    struct addrinfo hints;
    struct addrinfo *servinfo;  //will point to the results

    memset(&hints, 0, sizeof hints); //make sure the struct is empty
    hints.ai_family = AF_INET;  //ipv4
    hints.ai_socktype = SOCK_STREAM; //tcp


    //get server info, put into servinfo
    if ((status = getaddrinfo("192.168.2.3", port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return false;
    }


    //make socket
    fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (fd < 0) {
        printf("\nserver socket failure %m", errno);
        return false;
    }


    //connect
    if(connect(fd, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
        printf("\nclient connection failure %m", errno);
        return false;
    }


    return true;
}   //END LAUNCHCLIENT


/*Polls the robot for current sensor value every 15ms*/
void TcpClient::pollSensor() {
    while(1) {
        usleep(15000);
        Sensor_Packet temp;
        temp = myAgent->getRobot()->getSensorValue(myAgent->getRobot()->getCurrentSensor());
        UTILITY_H::lowsv = temp.values[0];
        UTILITY_H::highsv = temp.values[1];
    }   //end while
}   //END POLLSENSOR


/*Stores current position and goal and sends information to server*/
void TcpClient::updateServerAgent() {

    //hold message to get the length of it
    std::stringstream messagelength;
    //message is 1 prow pcol grow gcol sensorhigh sensorlow
    messagelength<<"1 "<<myAgent->getPosition().getRow()<<" "<<myAgent->getPosition().getCol()<<" "<<myAgent->getGoal().getRow()<<" "<<myAgent->getGoal().getCol()<<" "<<myAgent->getHighSV()<<" "<<myAgent->getLowSV();
    //make it into a string
    std::string tempStrLen = messagelength.str();


    int length_of_rest = 3;

    //find number of digits in prow
    while(isdigit(tempStrLen[length_of_rest]))
        length_of_rest++;

    //increment after prow
    length_of_rest++;

    //find number of digits in pcol
    while(isdigit(tempStrLen[length_of_rest]))
        length_of_rest++;


    //increment after pcol
    length_of_rest++;

    //find number of digits in grow
    while(isdigit(tempStrLen[length_of_rest]))
        length_of_rest++;

    //increment after grow
    length_of_rest++;

    //find number of digits in gcol
    while(isdigit(tempStrLen[length_of_rest]))
        length_of_rest++;

    //increment after gcol
    length_of_rest++;

    //find number of digits in first sensor value
    while(isdigit(tempStrLen[length_of_rest]) || tempStrLen[length_of_rest] == '-')
        length_of_rest++;

    //increment after first sensor value
    length_of_rest++;

    //find number of digits in second sensor value
    while(isdigit(tempStrLen[length_of_rest]) || tempStrLen[length_of_rest] == '-')
        length_of_rest++;


    //std::cout<<"\ntempStrLen: "<<tempStrLen;
    //std::cout<<"\nlength: "<<length_of_rest;

    //create message to send to server
    std::stringstream message;
    message<<"@ "<<length_of_rest<<" "<<messagelength.str();

    //std::cout<<"\nmessage: "<<message.str();


    int numSent = send(fd, message.str().c_str(), message.str().length(), 0);
}   //END UPDATESERVERAGENT

/*Processes a command from the server*/
void TcpClient::getCommand(char* command) {
    std::string tempCommand = command;

    //std::cout<<"\ncommand: "<<tempCommand;

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
        pthread_mutex_lock(&UTILITY_H::mutex_agent);

        //set new goal
        myAgent->setGoal(temp);
        //traverse
        Path new_path = myAgent->traverse(myAgent->getGoal());
        //add duplicate for bug
        new_path.getPath().insert(new_path.getPath().begin(), myAgent->getPosition());
        //set new path
        myAgent->setPath(new_path);

        //unlock
        pthread_mutex_unlock(&UTILITY_H::mutex_agent);


    }   //end if 1


    //id 2, change sensor
    else if(command[0] == '2') {

        //isolate id
        int idend = 3;
        while(isdigit(command[idend]))
            idend++;
        std::string temp = tempCommand.substr(2, idend-2);
        int id = atoi(temp.c_str());

        //check id
        //if invalid, just print and close function
        if(id < 7 || id > 42)
            std::cout<<"\n"<<id<<" is Invalid id";
        //else if valid, set new sensor and send message back to server
        else {

            myAgent->getRobot()->setCurrentSensor(id);

            //length of "2 id"
            int l = 2 + temp.length();
            //"- l ..."
            int length = 3 + l;

            //create message to send back to server
            std::stringstream message;
            message<<"@ "<<l<<" ";
            message<<command;

            //std::cout<<"\nSENSOR message: "<<message.str();


            int numSent = send(fd, message.str().c_str(), message.str().length(), 0);
        }   //end if valid sensor
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
        back[0] = '@';
        back[1] = ' ';
        back[2] = '1';
        back[3] = ' ';
        back[4] = '5';
        back[5] = '\0';

        int numSent = send(fd, back, 6, 0);
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
                //std::cout<<"\nServer: "<<in;
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
            //count number of headers
            for(int i=0;i<temp.str().length();i++)
                if(temp.str()[i] == '@')
                    numHeaders++;

            //check if exactly 1 header
            if(numHeaders == 1) {
                //send
                numSent = send(fd, out, 255, 0);
                //if an error, exit
                if(numSent < 0) {
                    printf("\nError sending %m", errno);
                    done = true;
                }   //end if an error
                //clear
            }   //end if headers == 1
            //clear out buffer
            memset(&out, 0, 255);

        }   //end if
    }   //end while
}   //END COMMUNICATE
