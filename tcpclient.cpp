
#include "tcpclient.h"
#include "agent.h"
#include <netinet/tcp.h>

TcpClient::TcpClient(char* p) : port(p), done(false), update_agent(new char[6]), myAgent(NULL) {}

TcpClient::~TcpClient() {}


/*Getter and setter for myAgent*/
void TcpClient::setAgent(Agent* a) {myAgent = a;}
Agent*& TcpClient::getAgent() {return myAgent;}


void TcpClient::setIP(char* addr) {ip_addr = addr;}
char* TcpClient::getIP() {return ip_addr;}


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



/*Stores current position and goal and sends information to server*/
void TcpClient::updateServerAgent() {

    //hold message to get the length of it
    std::stringstream messagelength;
    //message is 1 prow pcol grow gcol sensorhigh sensorlow
    messagelength<<"1 "<<myAgent->getPosition().getRow()<<" "<<myAgent->getPosition().getCol()<<" "<<myAgent->getGoal().getRow()<<" "<<myAgent->getGoal().getCol();
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


    //std::cout<<"\ntempStrLen: "<<tempStrLen;
    //std::cout<<"\nlength: "<<length_of_rest;

    //create message to send to server
    std::stringstream message;
    message<<"@ "<<messagelength.str();

    //std::cout<<"\nmessage: "<<message.str();


    int numSent = send(fd, message.str().c_str(), message.str().length(), 0);
}   //END UPDATESERVERAGENT


/*Processes a command from the server*/
void TcpClient::getCommand(char* command) {
    std::string tempCommand = command;
    //std::cout<<"\ncommand: "<<tempCommand;

    int num_headers = 0;
    for(int i=0;i<tempCommand.length() && num_headers < 2;i++)
        if(tempCommand[i] == '@')
            num_headers++;


    if(num_headers == 1) {


        //if id 6, update the path
        if(command[2] == '6') {

            int length_of_num = 1;
            while(isdigit(command[length_of_num+4]))
                length_of_num++;
            std::string num_str = tempCommand.substr(4, length_of_num);
            //std::cout<<"\nnum_str:"<<num_str;
            int num_pos = atoi(num_str.c_str());


            std::string list_of_pos = tempCommand.substr(5+length_of_num, tempCommand.length()- (5+length_of_num));
            std::cout<<"\nlist_of_pos:"<<list_of_pos;

            //lock
            pthread_mutex_lock(&UTILITY_H::mutex_agent);

            //clear the path
            myAgent->getPath().clear();

            int index = 1;
            for(int i=0;i<num_pos;i++) {

                //get the row
                int length_of_row = 0;
                while(isdigit(list_of_pos[length_of_row+index]))
                    length_of_row++;
                //std::cout<<"\nlengh of row: "<<length_of_row<<"\n";
                if(length_of_row==0)
                    length_of_row=1;
                //std::cout<<"\nlist_of_pos:"<<list_of_pos;
                //std::cout<<"\nllist_of_pos length:"<<list_of_pos.length();
                //std::cout<<"\nindex:"<<index;
                //std::cout<<"\nlength_of_row:"<<length_of_row;
                std::string row_str = list_of_pos.substr(index, length_of_row);
                //std::cout<<"\nrow_str: "<<row_str;

                //increment index. +1 for ','
                index += length_of_row+1;
                //std::cout<<"\nindex is now: "<<index;

                //get the column
                int length_of_col = 1;
                while(isdigit(list_of_pos[length_of_col+index]))
                    length_of_col++;
                std::string col_str = list_of_pos.substr(index, length_of_col);
                //std::cout<<"\ncol_str: "<<col_str;

                //increment index. +3 for ') ('
                index += length_of_col + 3;

                //make ints
                int row = atoi(row_str.c_str());
                int col = atoi(col_str.c_str());
                //make the Position
                Position temp(row, col);
                //std::cout<<"\ntemp: "<<temp.toString();

                myAgent->getPath().add(temp);

            }   //end for
            std::cout<<"\nNEW PATH: "<<myAgent->getPath().toString();

            //unlock
            pthread_mutex_unlock(&UTILITY_H::mutex_agent);
        }   //end if id 6


        //id 1, change goal
        else if(command[2] == '1') {

            //get row from command
            int rowend = 4;
            while(isdigit(command[rowend]))
                rowend++;
            std::string row_str = tempCommand.substr(4, rowend-3).c_str();
            //std::cout<<"\nrow_str: "<<row_str;
            int row = atoi(row_str.c_str());



            //get column from command
            int colstart = rowend+1;
            int colend = colstart+1;
            while(isdigit(command[colend]))
                colend++;
            std::string col_str = tempCommand.substr(colstart, colend-colstart);
            //std::cout<<"\ncol_str: "<<col_str;
            int col = atoi(col_str.c_str());

            //create temp position
            Position temp(row, col);
            //std::cout<<"\ntemp: "<<temp.toString();


            //std::cout<<"\nNEW GOAL VALUE: "<<myAgent->getGrid()->getPos(row, col);
            //std::cout<<"\nrow: "<<row<<" col: "<<col<<"\n";
            //std::cout<<"\ntemp: "<<temp.toString()<<"\n";

            //lock
            pthread_mutex_lock(&UTILITY_H::mutex_agent);

            //set new goal
            myAgent->setGoal(temp);

            //unlock
            pthread_mutex_unlock(&UTILITY_H::mutex_agent);


        }   //end if 1


        //id 2, change sensor
        else if(command[2] == '2') {

            //isolate id
            int idend = 4;
            while(isdigit(command[idend]))
                idend++;
            std::string temp = tempCommand.substr(4, idend-2);
            //std::cout<<"\ntemp: "<<temp;
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
                message<<"@ 2 ";
                //cut off the '@ l ' and add the rest
                message<<tempCommand.substr(4, tempCommand.length()-4);
                //std::cout<<"\nSENSOR message: "<<message.str();


                int numSent = send(fd, message.str().c_str(), message.str().length(), 0);
            }   //end if valid sensor
        }   //end if 2

        //id 3, toggle sensor streaming
        else if(command[2] == '3')
            myAgent->getRobot()->toggleSensorStream();
        //id 4, turn 360
        else if(command[2] == '4')
            myAgent->getRobot()->turnXDegrees(360);

        //id 5, quit
        else if(command[2] == '5') {
            char back[4];
            back[0] = '@';
            back[1] = ' ';
            back[2] = '5';
            back[3] = '\0';

            int numSent = send(fd, back, 4, 0);
            done = true;
        }   //end if 5
    }   //end if 1 header
    //else if not 1 header
    else
        std::cout<<"\nSTACKED MESSAGE";

}   //END GETCOMMAND


/*Non-blocking communication with server*/
void TcpClient::communicate() {

    fd_set read_flags,write_flags; // the flag sets to be used
    struct timeval waitd = {10, 0};          // the max wait time for an event
    int sel;                      // holds return value for select();

    int numSent;
    int numRead;
    char in[512];
    char out[255];
    memset(&in, 0, 512);
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
            memset(&in, 0, 512);

            numRead = recv(fd, in, 512, 0);
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
                std::cout<<"\nServer: "<<in;
                getCommand(in);
                memset(&in, 0, 512);
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
