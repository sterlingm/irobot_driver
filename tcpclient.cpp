
#include "tcpclient.h"
#include "agent.h"

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

    update_agent[0] = '1';
    update_agent[1] = (char)(((int)'0')+myAgent->getPosition().getRow());
    update_agent[2] = (char)(((int)'0')+myAgent->getPosition().getCol());
    update_agent[3] = (char)(((int)'0')+myAgent->getGoal().getRow());
    update_agent[4] = (char)(((int)'0')+myAgent->getGoal().getCol());
    update_agent[5] = '\0';


    int numSent = send(fd, update_agent, 6, 0);
}   //END UPDATESERVERAGENT


/*Processes a command from the server*/
void TcpClient::getCommand(char* command) {
    int numSent;
    //id 1, change goal
    if(command[0] == '1') {

        std::string tempCommand = command;
        std::cout<<"\ntempCommand: "<<tempCommand;

        int rowend = 2;

        while(isdigit(command[rowend+1]))
            rowend++;

        std::cout<<"\nrowend: "<<rowend;

        int colend = rowend+1;

        while(isdigit(command[colend+1]))
            colend++;


        std::cout<<"\ncolend final: "<<colend;


        int r = atoi(tempCommand.substr(1, rowend-1).c_str());
        std::cout<<"\nsubstr: "<<tempCommand.substr(1, rowend-1).c_str();
        std::cout<<"\nROW: "<<r;


        //have to change to get numbers > 10
        Position temp;
        int row = (int)command[2] - 48;
        temp.setRow(row);


        int col = (int)command[4] - 48;
        temp.setCol(col);

        //std::cout<<"\nrow: "<<row<<" col: "<<col<<"\n";
        //std::cout<<temp.toString()<<"\n";

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

        //updateserver
        updateServerAgent();

    }   //end if 1
    //id 2, change sensor
    else if(command[0] == '2') {

        //have to change for id > 10
        int id = (int)command[2] - 48;
        myAgent->getRobot()->setCurrentSensor(id);

        char* back = new char[3];
        back[0] = '2';
        back[1] = command[2];
        back[2] = '\0';

        numSent = send(fd, back, 3, 0);
        delete back;
    }   //end if 2
    //id 3, toggle sensor streaming
    else if(command[0] == '3')
        myAgent->getRobot()->toggleSensorStream();
    //id 4, turn 360
    else if(command[0] == '4')
        myAgent->getRobot()->turnXDegrees(360);
    //id 5, quit
    else if(command[0] == '5') {
        char* back = new char[2];
        back[0] = '5';
        back[1] = '\0';
        numSent = send(fd, back, 2, 0);
        done = true;
        delete back;
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
                cout<<"\nServer: "<<in;
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

            //send
            numSent = send(fd, out, 255, 0);
            //if an error, exit
            if(numSent < 0) {
                printf("\nError sending %m", errno);
                done = true;
            }   //end if an error
            //clear
            memset(&out, 0, 255);

        }   //end if
    }   //end while

    cout<<"\n\nExiting normally\n";
}   //END COMMUNICATE
