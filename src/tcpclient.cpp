
#include "tcpclient.h"
#include "robot_driver_agent.h"
#include <netinet/tcp.h>


TcpClient::TcpClient(char* p, char* ip, char ID, bool g) : port(p), ip_addr(ip), done(false), myAgent(0), id(ID), grid(0), grid_analyzer(0)
, gui(g) {}

TcpClient::~TcpClient() {
    if(grid != 0)
        delete grid;
    if(grid_analyzer != 0)
        delete grid_analyzer;
}

/*Getter and Setter for done*/
void TcpClient::setDone(bool d) {done = d;}
bool TcpClient::getDone() {return done;}

/*Getter and setter for myAgent*/
void TcpClient::setAgent(Agent* a) {myAgent = a;}
Agent*& TcpClient::getAgent() {return myAgent;}

/*Getter and setter for ip_addr*/
void TcpClient::setIP(char* addr) {ip_addr = addr;}
char* TcpClient::getIP() {return ip_addr;}

/*Getter and setter for read_mess*/
void TcpClient::set_read_mess(bool r) {read_mess = r;}
bool TcpClient::get_read_mess() {return read_mess;}

/*Getter and setter for sent_mess*/
void TcpClient::set_sent_mess(bool s) {sent_mess = s;}
bool TcpClient::get_sent_mess() {return sent_mess;}

bool TcpClient::getGUI() {return gui;}


/*Tries to connect to a server. Returns true if successful*/
bool TcpClient::launchClient() {
    int status, sock, adrlen;   //status, file descriptor, address length

    struct addrinfo hints;
    struct addrinfo *servinfo;  //will point to the results

    memset(&hints, 0, sizeof hints); //make sure the struct is empty
    hints.ai_family = AF_INET;  //ipv4
    hints.ai_socktype = SOCK_STREAM; //tcp


    //get server info, put into servinfo
    if ((status = getaddrinfo(ip_addr, port, &hints, &servinfo)) != 0) {
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

    if(!gui) {
        send_init_info();
        receive_grid_filename();
    }

    freeaddrinfo(servinfo);
    return true;
}   //END LAUNCHCLIENT


/*Sends the id to the server*/
void TcpClient::send_init_info() {
    //std::cout<<"\nattempting to send for client "<<id;
    std::stringstream to_send;
    to_send<<id<<" "<<myAgent->getCurrentSensor()<<" "<<myAgent->getRobot()->getDefaultVelocity()<<" "<<myAgent->get_algorithm();
    //std::cout<<"\nsending init message:"<<to_send.str()<<"\n";

    int num_sent = send(fd, to_send.str().c_str(), to_send.str().length(), 0);
    if(num_sent < 0)
        printf("\nError sending info: %m", errno);
}   //END SEND_ID


/*Receives the grid filename that the server is using*/
void TcpClient::receive_grid_filename() {
    char receive[255];
    int num_read = recv(fd, receive, 255, 0);
    receive[num_read] = '\0';

    if(num_read < 0)
        printf("\nError getting grid filename: %m", errno);

    else {
        std::string filename(receive);
        //std::cout<<"\nreceived "<<filename<<" as the filename";
        grid = new Grid(filename);
        grid_analyzer = new Grid_Analyzer(grid);
        myAgent->setGrid(grid);
        grid_analyzer->setGrid(grid);
        myAgent->setGridAnalyzer(grid_analyzer);
    }   //end else
}   //END RECEIVE_GRID_FILENAME


/*Stores current position and goal and sends information to server*/
void TcpClient::updateServerAgent() {

    //check pointer
    if(myAgent) {

        //lock
        pthread_mutex_lock(&mutex_agent);

        //hold message
        std::stringstream message;

        //message is 1 prow pcol grow gcol velocity direction
        message<<"@ 1 "<<myAgent->getPosition().getRow()<<" "<<myAgent->getPosition().getCol()<<" "<<myAgent->getGoal().getRow()<<" "
            <<myAgent->getGoal().getCol()<<" "<<myAgent->getRobot()->getDefaultVelocity()<<" "<<myAgent->getRobot()->getRealVelocity()
            <<" "<<myAgent->getDirection()<<" "<<myAgent->getRobot()->get_sensor_value(35).values[0];
        //std::cout<<"\nmessage: "<<message.str();

        //unlock
        pthread_mutex_unlock(&mutex_agent);

        //send
        int numSent = send(fd, message.str().c_str(), message.str().length(), 0);

        if(numSent > 0 && sent_mess)
            print_message(message.str().c_str());
        else if(numSent < 0)
            printf("\nError sending message: %s\nError message: %m", message.str().c_str(), errno);

    }   //end if valid pointer
}   //END UPDATESERVERAGENT


/*Processes a command from the server*/
void TcpClient::getCommand(char* command) {
    std::string tempCommand(command);
    //std::cout<<"\ncommand: "<<tempCommand<<"\n";

    //count the number of headers
    int num_headers = 0;
    for(int i=0;i<tempCommand.length() && num_headers < 2;i++)
        if(tempCommand[i] == '@')
            num_headers++;

    //if no stacking
    if(num_headers == 1) {

        //if id 6, update the path
        if(tempCommand[2] == '6') {

            int length_of_num = 1;
            while(isdigit(command[length_of_num+4]))
                length_of_num++;
            std::string num_str(tempCommand.substr(4, length_of_num));
            //std::cout<<"\nnum_str:"<<num_str;
            int num_pos = atoi(num_str.c_str());

            //get the list of positions
            std::string list_of_pos(tempCommand.substr(5+length_of_num, tempCommand.length()- (5+length_of_num)));
            //std::cout<<"\nlist_of_pos:"<<list_of_pos;

            //lock
            pthread_mutex_lock(&mutex_agent);

            //clear the path
            myAgent->getPath().clear();

            //isolate each position and add it to the path
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
                //std::cout<<"\nlength_of_row:"<<length_of_row<<"\n";
                std::string row_str(list_of_pos.substr(index, length_of_row));
                //std::cout<<"\nrow_str: "<<row_str;

                //increment index. +1 for ','
                index += length_of_row+1;
                //std::cout<<"\nindex is now: "<<index;

                //get the column
                int length_of_col = 1;
                while(isdigit(list_of_pos[length_of_col+index]))
                    length_of_col++;
                std::string col_str(list_of_pos.substr(index, length_of_col));
                //std::cout<<"\ncol_str: "<<col_str;

                //increment index. +3 for ') ('
                index += length_of_col + 3;

                //make ints
                int row = atoi(row_str.c_str());
                int col = atoi(col_str.c_str());
                //make the Position
                Position temp(row, col);
                //std::cout<<"\ntemp: "<<temp.toString()<<"\n";

                if(myAgent) {
                    //add position to the path
                    myAgent->getPath().add(temp);
                    usleep(1000);
                }   //end if
            }   //end for
            //std::cout<<"\nNEW PATH: "<<myAgent->getPath().toString();

            //unlock
            pthread_mutex_unlock(&mutex_agent);

        }   //end if id 6

        //id g, change goal
        else if(tempCommand[2] == 'g') {

            //get row from command
            int rowend = 4;
            while(isdigit(command[rowend]))
                rowend++;
            std::string row_str(tempCommand.substr(4, rowend-3).c_str());
            //std::cout<<"\nrow_str: "<<row_str;
            int row = atoi(row_str.c_str());

            //get column from command
            int colstart = rowend+1;
            int colend = colstart+1;
            while(isdigit(command[colend]) || command[colend] == '-')
                colend++;
            std::string col_str(tempCommand.substr(colstart, colend-colstart));
            //std::cout<<"\ncol_str: "<<col_str;
            int col = atoi(col_str.c_str());

            //create temp position
            Position temp(row, col);
            //std::cout<<"\ntemp: "<<temp.toString();


            //std::cout<<"\nNEW GOAL VALUE: "<<myAgent->getGrid()->getPos(row, col);
            //std::cout<<"\nrow: "<<row<<" col: "<<col<<"\n";

            //lock
            pthread_mutex_lock(&mutex_agent);

            //check pointer
            if(myAgent)
                //set new goal
                myAgent->setGoal(temp);
                //std::cout<<"\nnew goal is now "<<myAgent->getGoal().toString();

            //unlock
            pthread_mutex_unlock(&mutex_agent);
        }   //end if 1

        //id s, change sensor
        else if(tempCommand[2] == 's') {

            //isolate id
            int idend = 4;
            while(isdigit(command[idend]))
                idend++;
            std::string temp(tempCommand.substr(4, idend-2));
            //std::cout<<"\ntemp: "<<temp;
            int id = atoi(temp.c_str());

            //check id
            //if invalid, just print and close function
            if(id < 7 || id > 42)
                std::cout<<"\n"<<id<<" is Invalid id";

            //else if valid, set new sensor and send message back to server
            else {

                //check pointer
                if(myAgent)
                    myAgent->setCurrentSensor(id);

                //create message to send back to server
                std::stringstream message;
                //add the message id and sensor id
                message<<"@ 2 "<<temp;
                //std::cout<<"\nSENSOR message: "<<message.str();

                int numSent = send(fd, message.str().c_str(), message.str().length(), 0);
            }   //end if valid sensor
        }   //end if 2

        //id p, turn 360
        else if(tempCommand[2] == 'p' && myAgent) {

            //wait to finish driving
            while(myAgent->is_driving()) {}

            //lock
            pthread_mutex_lock(&mutex_agent);
            //set spinning to true
            myAgent->set_spinning(true);
            //spin
            myAgent->getRobot()->turnXDegrees(360);
            //change spinning back
            myAgent->set_spinning(false);
            //unlock
            pthread_mutex_unlock(&mutex_agent);
        }

        else if(tempCommand[2] == 'v' && myAgent) {

            //isolate velocity
            int idend = 4;
            while(isdigit(command[idend]) || command[idend] == '-')
                idend++;
            std::string temp(tempCommand.substr(4, idend-4));
            //std::cout<<"\ntemp: "<<temp;
            int velocity = atoi(temp.c_str());
            //std::cout<<"\nvel: "<<velocity;


            //lock
            pthread_mutex_lock(&mutex_agent);

            myAgent->getRobot()->setDefaultVelocity(velocity);

            //unlock
            pthread_mutex_unlock(&mutex_agent);
        }   //end change velocity

        //if changing direction
        else if(tempCommand[2] == 'd' && myAgent) {
            char temp[3];
            temp[0] = tempCommand[4];
            //check for diagonal direction
            if(tempCommand.length() == 7) {
                temp[1] = tempCommand[5];
                temp[2] = '\0';
            }
            else
                temp[1] = '\0';
            //std::cout<<"\ntemp:"<<temp;

            int d;
            //if diagonal direction
            if(temp[1] != '\0') {
                if( ((temp[0] == 'n' || temp[0] == 'N') && (temp[1] == 'e' || temp[1] == 'E')) )
                    d = NORTHEAST;
                else if( ((temp[0] == 'n' || temp[0] == 'N') && (temp[1] == 'w' || temp[1] == 'W')) )
                    d = NORTHWEST;
                else if( ((temp[0] == 's' || temp[0] == 'S') && (temp[1] == 'e' || temp[1] == 'E')) )
                    d = SOUTHEAST;
                else if( ((temp[0] == 's' || temp[0] == 'S') && (temp[1] == 'w' || temp[1] == 'W')) )
                    d = SOUTHWEST;

                //lock
                pthread_mutex_lock(&mutex_agent);

                myAgent->change_direction(d);

                //unlock
                pthread_mutex_unlock(&mutex_agent);
            }   //end if diagonal
            //else
            else {
                if(temp[0] == 'n' || temp[0] == 'N')
                    d = NORTH;
                else if(temp[0] == 's' || temp[0] == 'S')
                    d = SOUTH;
                else if(temp[0] == 'e' || temp[0] == 'E')
                    d = EAST;
                else if(temp[0] == 'w' || temp[0] == 'W')
                    d = WEST;

                //lock
                pthread_mutex_lock(&mutex_agent);

                myAgent->change_direction(d);

                //unlock
                pthread_mutex_unlock(&mutex_agent);
            }   //end else
        }   //end if changing direction

        else if(tempCommand[2] == 'm' && myAgent) {
            char m = tempCommand[4];
            if(m == 'f')
                myAgent->getRobot()->fullMode();
            else if(m == 's')
                myAgent->getRobot()->safeMode();
        }

        //id q, quit
        else if(tempCommand[2] == 'q') {
            char back[4];
            back[0] = '@';
            back[1] = ' ';
            back[2] = '5';
            back[3] = '\0';

            int numSent = send(fd, back, 4, 0);
            done = true;
        }   //end if 5

        //id t, toggle sensor streaming
        else if(tempCommand[2] == 't' && myAgent)
            myAgent->getRobot()->toggleSensorStream();


        //print message
        if(read_mess)
            print_message(command);
    }   //end if 1 header
    //else if not 1 header
    //else
        //std::cout<<"\nSTACKED MESSAGE - NOTHING WILL BE DONE FOR MESSAGE: "<<tempCommand;

}   //END GETCOMMAND

/*Prints out a message about the passed command*/
void TcpClient::print_message(char* command) {
    //std::cout<<"\ncommand:"<<command;
    std::string temp_command = command;

    char id = temp_command[2];
    //std::cout<<"\nid:"<<id<<"\n";

    //if update path command
    if(id == '6') {
        int length = 0;
        while(isdigit(temp_command[4+length]))
            length++;
        std::cout<<"\nReceived command to update path to: "<<temp_command.substr(5+length, strlen(temp_command.c_str())-(5+length));
    }   //end if 6

    //if change goal command
    else if(id == 'g') {

        //isolate row
        int row_index=4;
        while(isdigit(temp_command[row_index]))
            row_index++;
        std::string row(temp_command.substr(4, row_index-4));
        //std::cout<<"\nrow:"<<row;

        //isolate column
        int col_index=row_index+1;
        while(isdigit(temp_command[col_index]))
            col_index++;
        std::string col(temp_command.substr(row_index+1,col_index-(row_index+1)));
        //std::cout<<"\ncol:"<<col;

        std::cout<<"\nReceived command to change goal to ("<<row<<","<<col<<")";
    }   //end if change goal command

    //if change sensor command
    else if(id == 's') {

        //isolate sensor id
        int index = 4;
        while(isdigit(temp_command[index]))
            index++;
        std::string sen(temp_command.substr(4, index-4));
        //std::cout<<"\nsen:"<<sen;

        std::cout<<"\nReceived command to change the sensor to id "<<sen;
    }   //end if sensor command

    else if(id == 'v') {

        //isolate velocity
        int index = 4;
        while(isdigit(temp_command[index]) || temp_command[index] == '-')
            index++;
        std::string vel(temp_command.substr(4, index-4));
        //std::cout<<"\nvel:"<<vel;

        std::cout<<"\nReceived command to change the velocity to "<<vel;
    }
    else if(id == 'd')
        std::cout<<"\nReceived command to change direction to "<<temp_command[4];

    else if(id == 't')
        std::cout<<"\nReceived command to toggle sensors";

    else if(id == 'p')
        std::cout<<"\nReceived command to spin";
    else if(id == 'q')
        std::cout<<"\nReceived command to quit";

    else if(id == '1') {
        std::cout<<"\nMessage sent to update the server with information - ";
        std::cout<<"position: "<<myAgent->getPosition().toString()<<" goal: "<<myAgent->getGoal().toString();
        std::cout<<" direction: "<<myAgent->getDirection();
    }
    //else
        //std::cout<<"\nelse message: "<<command;
}   //END PRINTMESSAGE



void TcpClient::get_gui_command() {
    char in[255];
    int num_read = recv(fd, in, 255, 0);
    in[num_read] = '\0';
    std::string cmd(in);

    //count the number of headers
    int num_headers = 0;
    for(int i=0;i<cmd.length() && num_headers < 2;i++)
        if(cmd[i] == '@')
            num_headers++;

    if(num_headers == 1) {

        //FULL MODE BUTTON
        if(cmd[2] == '1')
            myAgent->getRobot()->fullMode();

        //SAFE MODE BUTTON
        else if(cmd[2] == '2')
            myAgent->getRobot()->safeMode();

        //STOP BUTTON
        else if(cmd[2] == '3')
            myAgent->getRobot()->stop();

        //TOGGLE SENSOR STREAM BUTTON
        else if(cmd[2] == '9')
            myAgent->getRobot()->toggleSensorStream();

        //DRIVE BUTTON
        else if(cmd[2] == '4') {
            int v_index = 4;
            while(isdigit(cmd[v_index]) || cmd[v_index] == '-')
                v_index++;
            std::string v_str = cmd.substr(4, v_index - 4);
            //std::cout<<"\nv_str:"<<v_str;
            int velocity = atoi(v_str.c_str());

            int r_index = v_index+1;
            while(isdigit(cmd[r_index]) || cmd[r_index] == '-')
                r_index++;
            std::string r_str = cmd.substr(v_index+1, r_index - v_index);
            //std::cout<<"\nr_str:"<<r_str;
            int radius = atoi(r_str.c_str());


            //if the radius has a value, drive at a radius
            if(radius != 0)
                myAgent->getRobot()->drive(velocity, radius);
            //if not, drive straight
            else
                myAgent->getRobot()->drive_straight(velocity);
        }   //end drive

        //TURN BUTTON
        else if(cmd[2] == '5') {

            int t_index = 4;
            while(isdigit(cmd[t_index]) || cmd[t_index] == '-')
                t_index++;
            std::string t_str = cmd.substr(4, t_index - 4);
            //std::cout<<"\nt_str:"<<t_str;
            int turn = atoi(t_str.c_str());

            int v_index = t_index+1;
            while(isdigit(cmd[v_index]) || cmd[v_index] == '-')
                v_index++;
            std::string v_str = cmd.substr(t_index+1, v_index - t_index);
            //std::cout<<"\nv_str:"<<v_str;
            int velocity = atoi(v_str.c_str());

            int s_index = v_index+1;
            while(isdigit(cmd[s_index]))
                s_index++;
            std::string s_str = cmd.substr(v_index+1, s_index - v_index);
            //std::cout<<"\ns_str:"<<s_str;
            int seconds = atoi(s_str.c_str());

            //only get the velocity in case seconds has no value
            //if seconds has >0 value, the velocity is calculated
            //depending on seconds and turn degrees

            //if seconds has a positive value, turn x degrees in y seconds
            if(seconds > 0)
                myAgent->getRobot()->turnXDegreesInYSeconds(turn, seconds);
            //if not, turn x degrees
            else
                myAgent->getRobot()->turnXDegrees(turn, velocity);
        }   //end if turn button

        //TURN CLOCKWISE BUTTON
        else if(cmd[2] == '6') {
            int v_index = 4;
            while(isdigit(cmd[v_index]) || cmd[v_index] == '-')
                v_index++;
            std::string v_str = cmd.substr(4, v_index - 4);
            //std::cout<<"\nv_str:"<<v_str;
            int velocity = atoi(v_str.c_str());

            myAgent->getRobot()->turnClockwise(velocity);
        }   //end turncw button

        //TURN COUNTER-CLOCKWISE BUTTON
        else if(cmd[2] == '7') {
            int v_index = 4;
            while(isdigit(cmd[v_index]) || cmd[v_index] == '-')
                v_index++;
            std::string v_str = cmd.substr(4, v_index - 4);
            //std::cout<<"\nv_str:"<<v_str;
            int velocity = atoi(v_str.c_str());

            myAgent->getRobot()->turnCounterClockwise(velocity);
        }   //end turnccw button

        //LED BUTTON
        else if(cmd[2] == '8') {
            //std::cout<<"\ncmd:"<<cmd;
            bool play = false;
            bool advance = false;

            //get play checkbox
            int p_index = 4;
            while(isdigit(cmd[p_index]))
                p_index++;
            std::string p_str = cmd.substr(4, p_index - 4);
            //std::cout<<"\np_str:"<<p_str;
            if(atoi(p_str.c_str()) == 1)
                play = true;

            //get advance checkbox
            int adv_index = p_index+1;
            while(isdigit(cmd[adv_index]))
                adv_index++;
            std::string adv_str = cmd.substr(p_index+1, adv_index - p_index);
            //std::cout<<"\nadv_str:"<<adv_str;
            if(atoi(adv_str.c_str()) == 1)
                advance = true;

            //get color value
            int c_index = adv_index+1;
            while(isdigit(cmd[c_index]) || cmd[c_index] == '.')
                c_index++;
            std::string c_str = cmd.substr(adv_index+1, c_index - adv_index);
            //std::cout<<"\nc_str:"<<c_str;
            double color = atof(c_str.c_str());

            //get intensity checkbox
            int i_index = c_index+1;
            while(isdigit(cmd[i_index]) || cmd[i_index] == '.')
                i_index++;
            std::string i_str = cmd.substr(c_index+1, i_index - c_index);
            //std::cout<<"\ni_str:"<<i_str;
            double intensity = atof(i_str.c_str());

            myAgent->getRobot()->leds(play, advance, (unsigned char)color, (unsigned char)intensity);
        }   //end if led button

        //CHOICE SELECTED
        else if(cmd[2] == '0') {
            int s_index = 4;
            while(isdigit(cmd[s_index]))
                s_index++;
            std::string s_str = cmd.substr(4, s_index - 4);
            //std::cout<<"\ns_str:"<<s_str;
            int sensor = atoi(s_str.c_str());

            myAgent->setCurrentSensor(sensor);
        }   //end if choice selected

        else if(cmd[2] == 'q') {
            done = true;
            myAgent->getRobot()->stop();
            myAgent->getRobot()->pauseSensorStream();
            myAgent->getRobot()->leds(false, false, 0, 0);
        }   //end if quit
    }   //end if headers==1
}   //END GET_GUI_COMMAND




/*Non-blocking communication with server*/
void TcpClient::communicate() {

    fd_set read_flags, write_flags; // the flag sets to be used
    int sel;        //holds return value for select();
    int numSent;    //holds return value for send()
    int numRead;    //holds return value for read()
    char in[512];   //in buffer
    char out[255];  //out buffer

    //clear buffers
    memset(&in, 0, 512);
    memset(&out, 0, 255);


    while(!done) {

        FD_ZERO(&read_flags);
        FD_ZERO(&write_flags);
        FD_SET(fd, &read_flags);
        FD_SET(fd, &write_flags);
        FD_SET(STDIN_FILENO, &read_flags);
        FD_SET(STDIN_FILENO, &write_flags);

        //call select
        sel = select(fd+1, &read_flags, &write_flags, (fd_set*)0, 0);

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
            if(numRead < 0) {
                printf("\nError reading from Server: %m", errno);
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


            //check for message corruptness
            int numHeaders = 0;
            std::string temp(out);
            //count number of headers
            for(int i=0;i<temp.length();i++)
                if(temp[i] == '@')
                    numHeaders++;

            //check if exactly 1 header
            if(numHeaders == 1) {
                //send
                numSent = send(fd, out, 255, 0);
                //if an error, exit
                if(numSent < 0) {
                    printf("\nError sending message: %s\nError message: %m", out, errno);
                    done = true;
                }   //end if an error
                else if(numSent > 0 && sent_mess)
                    print_message(out);
            }   //end if headers == 1

            //clear out buffer
            memset(&out, 0, 255);
        }   //end if
    }   //end while
}   //END COMMUNICATE
