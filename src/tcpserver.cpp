#include "tcpserver.h"
#include "robot_driver_agent.h"
#include <netinet/tcp.h>


TcpServer::TcpServer(char* p, char* ip, int c, std::string gf) : port(p), done(false), ip_addr(ip), count(c), grid_filename(gf) {
    clients = new client_info[count];
    //std::cout<<"\ncount: "<<count;
}

TcpServer::~TcpServer() {delete [] clients; clients = 0;}

/*Getter and setter for done*/
bool TcpServer::getDone() {return done;}
void TcpServer::setDone(bool d) {done = d;}

/*Getter and setter for read_mess*/
void TcpServer::set_read_mess(bool r) {read_mess = r;}
bool TcpServer::get_read_mess() {return read_mess;}

/*Getter and setter for sent_mess*/
void TcpServer::set_sent_mess(bool s) {sent_mess = s;}
bool TcpServer::get_sent_mess() {return sent_mess;}


/*Getter and setter for ip_addr*/
void TcpServer::setIP(char* addr) {ip_addr = addr;}
char* TcpServer::getIP() {return ip_addr;}

/*Returns the value of which_display*/
char TcpServer::getWhichDisplay() {return which_display;}

/*Getter and setter for grid_filename*/
std::string TcpServer::getGridFilename() {return grid_filename;}
void TcpServer::setGridFilename(std::string gf) {grid_filename = gf;}


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

    //accept the connections
    for(int i=0;i<count;i++) {
        std::cout<<"\nWaiting for "<<count-i<<" client(s)\n";
        clients[i].fd = accept(fd, (struct sockaddr*)&their_addr, &their_addr_size);
        if(clients[i].fd < 0) {
            printf("\nError accepting client %i: %m", i+1, errno);
            return false;
        }   //end if
        //if gui
        if(strcmp(grid_filename.c_str(), "") == 0) {
            cs.push_back(35);
            velocities.push_back(0);
            algos.push_back(1);
        }   //end if
        //else if no gui
        else {
            recv_client_init_info(i);
            send_grid_filename(i, grid_filename);
            std::cout<<"Client "<<clients[i].id<<" connected!";
        }   //end else
    }   //end for

    //if no gui, default which_display to the first client connected
    if(strcmp(grid_filename.c_str(), "") != 0)
        which_display = clients[0].id;


    //for(int i=0;i<count;i++)
        //std::cout<<"\nClient "<<i+1<<" fd: "<<clients[i].fd<<" id:"<<clients[i].id<<" agent"<<clients[i].agent;
    return true;
}   //END LAUNCHSERVER


/*Gets the init from a client*/
void TcpServer::recv_client_init_info(int& i) {
    char temp[11];
    int num_read = recv(clients[i].fd, temp, 10, 0);
    temp[10] = '\0';
    //std::cout<<"\nreceived init message:"<<temp<<"\n";
    if(num_read < 0)
        printf("\nError reading client %i's id: %m", i, errno);
    else {

        //set id
        clients[i].id = temp[0];

        //hold char* to substr
        std::string hold(temp);

        //get the initial sensor id
        //can be 2 digits so have to do index
        int is_index = 2;
        while(isdigit(temp[is_index]))
            is_index++;
        //isolate id with substr
        std::string s_id = hold.substr(2, is_index-2);
        //push back value
        cs.push_back(atoi(s_id.c_str()));

        //get velocity value
        int v_index=is_index+1;
        while(isdigit(temp[v_index]))
            v_index++;
        std::string v = hold.substr(is_index+1, v_index-is_index);
        velocities.push_back(atoi(v.c_str()));

        int a_index = v_index+1;
        while(isdigit(temp[a_index]))
            a_index++;
        std::string a = hold.substr(v_index+1, a_index-v_index);
        //std::cout<<"\na:"<<a;
        algos.push_back(atoi(a.c_str()));
    }
    //else
        //std::cout<<"\nclient "<<i<<"'s id:"<<clients[i].id;
}   //END RECVCLIENTID


/*Sends the grid filename to client at index i*/
void TcpServer::send_grid_filename(int& i, std::string g) {
    //std::cout<<"\nsending:"<<g;
    int num_sent = send(clients[i].fd, g.c_str(), g.length(), 0);
    if(num_sent < 0)
        printf("\nError sending grid filename to cliend %i: %m", i, errno);
}   //END SEND_GRID_FILENAME


/*Get the file descriptor of a client given the client's id*/
int& TcpServer::get_client_fd(char& id) {
    for(int i=0;i<count;i++)
        if(clients[i].id == id)
            return clients[i].fd;
}   //END GETCLIENTFD


/*Get the id of a client given its fd*/
char& TcpServer::get_client_id(int& fd) {
    for(int i=0;i<count;i++)
        if(clients[i].fd == fd)
            return clients[i].id;
}   //END GETCLIENTID


/*Get the client_info struct given an id*/
client_info& TcpServer::get_client(char& id) {
    for(int i=0;i<count;i++)
        if(clients[i].id == id)
            return clients[i];
}   //END GETCLIENT

/*Get the client_info struct given a fd*/
client_info& TcpServer::get_client(int& fd) {
    for(int i=0;i<count;i++)
        if(clients[i].fd == fd)
            return clients[i];
}   //END GETCLIENTINFO


/*Sets the agent member in each client_info of the client_info array*/
void TcpServer::set_agents(std::vector<Agent>& a_vec) {
    if(a_vec.size() == count)
        for(int i=0;i<count;i++) {
            //std::cout<<"\ni:"<<i<<"\n";
            //std::cout<<"\nsetting clients["<<i<<"] to "<<&a_vec.at(i)<<"\n";
            clients[i].agent = &a_vec.at(i);
            clients[i].agent->setCurrentSensor(cs.at(i));
            clients[i].agent->getRobot()->setVelocity(velocities.at(i));
            clients[i].agent->set_algorithm(algos.at(i));
        }
}   //END SETAGENTS

/*Returns true if c is an id*/
bool TcpServer::is_id(char& id) {
    for(int i=0;i<count;i++)
        if(clients[i].id == id)
            return true;
    return false;
}   //END ISID


int TcpServer::get_client_index(char id) {
    for(int i=0;i<count;i++)
        if(clients[i].id == id)
            return i;
    return -1;
}

/*Returns the array of client_info*/
client_info*& TcpServer::get_clients() {return clients;}

/*Returns count*/
const int TcpServer::get_num_clients() {return count;}

/*Gets a message sent back from client in response to a previously sent command*/
void TcpServer::getSendBack(char* command, char client_id) {
    std::string tempCommand(command);
    //std::cout<<"\ntempCommand: "<<tempCommand;

    //count the number of headers
    int num_headers = 0;
    for(int i=0;i<tempCommand.length() && num_headers < 2;i++)
        if(tempCommand[i] == '@')
            num_headers++;

    //if no message stacking
    if(num_headers == 1) {

        //if packet id 1 (update pos, goal, velocity, direction)
        if(tempCommand[2] == '1') {

            //get position row
            int prowend = 4;
            while(isdigit(tempCommand[prowend]))
                prowend++;
            std::string p(tempCommand.substr(4, prowend-3));
            int prow = atoi(p.c_str());
            //std::cout<<"\nprow: "<<prow;

            //get position column
            int pcolend = prowend+1;
            while(isdigit(tempCommand[pcolend]))
                pcolend++;
            std::string c(tempCommand.substr(prowend, pcolend-prowend));
            int pcol = atoi(c.c_str());
            //std::cout<<"\npcol: "<<pcol;

            //get goal row
            int growend = pcolend+1;
            while(isdigit(tempCommand[growend]))
                growend++;
            std::string gr(tempCommand.substr(pcolend, growend-pcolend));
            int grow = atoi(gr.c_str());
            //std::cout<<"\ngrow: "<<grow;

            //get goal column
            int gcolend = growend+1;
            while(isdigit(tempCommand[gcolend]))
                gcolend++;
            std::string gc(tempCommand.substr(growend, gcolend-growend));
            int gcol = atoi(gc.c_str());
            //std::cout<<"\ngcol: "<<gcol;

            //get velocity
            int vend = gcolend+1;
            while(isdigit(tempCommand[vend]) || tempCommand[vend] == '-')
                vend++;
            std::string vstr(tempCommand.substr(gcolend, vend-gcolend));
            int velocity = atoi(vstr.c_str());
            //std::cout<<"\nv: "<<velocity;

            int dend = vend+1;
            while(isdigit(tempCommand[dend]))
                dend++;
            std::string dstr(tempCommand.substr(vend, dend-vend));
            //std::cout<<"\ndstr:"<<dstr;
            int direction = atoi(dstr.c_str());
            //std::cout<<"\ndirection: "<<direction;

            int mend = dend+1;
            while(isdigit(tempCommand[mend]) || tempCommand[mend] == '-')
                mend++;
            std::string mstr(tempCommand.substr(dend+1, mend-dend));    //give +1 because not doing atoi
            //std::cout<<"\nmstr:"<<mstr;
            char mode;
            if(mstr == "3") mode = 'f';
            else if(mstr == "2") mode = 's';
            else if(mstr == "1") mode = 'p';
            else mode = get_client(client_id).agent->get_mode();
            //std::cout<<"\nmode:"<<mode;


            Position temp_pos(prow, pcol);
            Position temp_goal(grow, gcol);

            //lock
            pthread_mutex_lock(&mutex_agent);

            //set the new information for the client
            get_client(client_id).agent->setPosition(temp_pos);
            get_client(client_id).agent->setGoal(temp_goal);
            get_client(client_id).agent->getRobot()->setVelocity(velocity);
            get_client(client_id).agent->setDirection(direction);
            get_client(client_id).agent->set_mode(mode);

            //unlock
            pthread_mutex_unlock(&mutex_agent);
        }   //end if update

        //else if packet id 2 (change sensor)
        else if(tempCommand[2] == '2') {
            int idend = 4;
            while(isdigit(tempCommand[idend]))
                idend++;
            std::string temp(tempCommand.substr(4, idend-2));

            //std::cout<<"\ntemp: "<<temp;
            int id = atoi(temp.c_str());

            //if valid id value, set new current sensor
            if(id > 6 && id < 43)
                get_client(client_id).agent->setCurrentSensor(id);
            else
                std::cout<<"\n"<<id<<" is Invalid id";
        }   //end if new sensor

        //else if 5, quit
        else if(tempCommand[2] == '5')
            done = true;

    }   //end if 1 header
    //else if stacking
    //else
        //std::cout<<"\nSTACKED MESSAGE: "<<tempCommand;
}   //END GETSENDBACK



/*Sends the updated path to the client*/
void TcpServer::sendPath(Path& p, char client_id) {

    //message to send
    std::ostringstream tosend;
    //add header and id and number of positions in the path
    tosend<<"@ 6 "<<p.getSize()<<" ";

    //get the list of positions in separate stringstream
    std::stringstream list_of_pos;
    for(int i=0;i<p.getSize()-1;i++)
        list_of_pos<<p.getPathVector().at(i).toString()<<" ";
    //add last position with no space on the end
    list_of_pos<<p.getPathVector().back().toString();

    //std::cout<<"\nlist_of_pos: "<<list_of_pos.str()<<"\n";

    //concatenate the list
    tosend<<list_of_pos.str();
    //std::cout<<"\ntosend: "<<tosend.str()<<"\n";

    //send
    int num_sent = send(get_client_fd(client_id), tosend.str().c_str(), tosend.str().length(), 0);

    if(num_sent > 0 && sent_mess)
        std::cout<<"\nPath successfully sent to Client "<<client_id<<": "<<tosend.str();
}   //END SENDPATH



/*Non-blocking communication with client*/
void TcpServer::communicate() {

    fd_set read_flags, write_flags; // the flag sets to be used
    int sel;        // holds return value for select();
    int numRead = 0;    //holds return value for read()
    int numSent = 0;    //holds return value for send()
    char in[count][255];   //in buffers for clients
    char out[512];  //out buffer


    //clear buffers
    memset(&out, 0, 512);
    for(int i=0;i<count;i++)
        memset(&in[i], 0, 255);

    //count fds
    int nfds = 0;
    for(int i=0;i<count;i++)
        nfds += clients[i].fd;

    while(!done) {
        //reset the fd_sets
        FD_ZERO(&read_flags);
        FD_ZERO(&write_flags);
        FD_SET(STDIN_FILENO, &read_flags);
        FD_SET(STDIN_FILENO, &write_flags);

        //put fds back in
        for(int i=0;i<count;i++) {
            FD_SET(clients[i].fd, &read_flags);
            FD_SET(clients[i].fd, &write_flags);
        }   //end for

        //call select
        sel = select(nfds+1, &read_flags, &write_flags, (fd_set*)0, 0);

        //if an error with select
        if(sel < 0)
            continue;

        //loop through clients
        for(int i=0;i<count;i++) {
            //check if ready for reading
            if(FD_ISSET(clients[i].fd, &read_flags)) {

                //clear set
                FD_CLR(clients[i].fd, &read_flags);

                //clear in
                for(int i=0;i<count;i++)
                    memset(&in[i], 0, 255);



                numRead = recv(clients[i].fd, in[i], 255, 0);
                if(numRead < 0) {
                    printf("\nError reading from Client 1: %m", errno);
                    clients[i].agent->getRobot()->pauseSensorStream();
                    done = true;
                }   //end if error
                //if connection closed, exit
                else if(numRead == 0) {
                    printf("\nClosing socket");
                    close(clients[i].fd);
                    done = true;
                    i = count;
                }   //end if connection closed
                //if message, call getsendback
                else if(in[i][0] != '\0') {
                    if(read_mess)
                        std::cout<<"\nMessage successfully received from Client "<<clients[i].id<<": "<<in[i];
                    getSendBack(in[i], clients[i].id);
                }   //end if message
            }   //end if
        }   //end for



        /*CHECK FOR STDIN*/

        //if stdin is ready for reading
        if(FD_ISSET(STDIN_FILENO, &read_flags)) {
            fgets(out, 512, stdin);

            //if changing the view
            if(out[0] == 'r') {
                std::string temp(out);
                char w = temp.substr(2, 1)[0];
                if(is_id(w))
                    which_display = w;
            }   //end if changing the view

            //else check for writing
            else {

                for(int i=0;i<count;i++) {
                    //if a socket is reading for writing
                    if(FD_ISSET(clients[i].fd, &write_flags)) {

                        //clear set
                        FD_CLR(clients[i].fd, &write_flags);

                        //if not changing the view and begins with a digit or quitting
                        if(is_id(out[0]) || out[0] == 'q') {

                            //create message to send
                            std::stringstream tosend;

                            //if not the quit command
                            if(out[0] != 'q') {

                                //make a temp of out to substr out the client id
                                std::string temp(out);
                                tosend<<"@ "<<temp.substr(2, temp.length() - 2);
                                //std::cout<<"\ntosend: "<<tosend.str();

                                //send message to the client
                                numSent = send(get_client_fd(out[0]), tosend.str().c_str(), tosend.str().length(), 0);

                            }   //end if not the quit command

                            //if quit command
                            else {

                                tosend<<"@ q";
                                //std::cout<<"\ntosend: "<<tosend.str();

                                //send message to quit to all clients
                                for(int i=0;i<count;i++)
                                    numSent = send(clients[i].fd, tosend.str().c_str(), tosend.str().length(), 0);

                            }   //end quit message

                            //if error, exit
                            if(numSent < 0) {
                                printf("\nError sending to Client %c\nMessage: %s\nError message %m", out[0], out, errno);
                                done = true;
                                i = count;
                            }   //end if error

                            //if no error and sent_mess is true, print message
                            else if(numSent > 0 && sent_mess)
                                std::cout<<"\nMessage successfully sent to Client "<<out[0]<<": "<<tosend.str();

                        }   //end if not changing view and begins with a digit or quitting

                        //wait for message to get there, then clear
                        usleep(10000);
                        memset(&out, 0, 512);
                    }   //end if
                }   //end for
            }   //end if not changing view
        }   //end if stdin ready for reading
    }   //end while
}   //END COMMUNICATE
