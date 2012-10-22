
#include "robot_driver_utility.h"
#include "robot_driver_grid.h"
#include "guiwindow.h"
#include "robot.h"
#include "robot_driver_agent.h"
#include "clientcontrol.h"
#include "servercontrol.h"
#include "robot_driver_tree.h"
#include "tcpserver.h"
#include "tcpclient.h"
#include "owncontrol.h"
#include "grid_analyzer.h"
#include <libfreenect.hpp>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <string>




using namespace std;

string mode;
string grid_filename = "grids/";
string rest;
string ip;
string id;
string client_count;
string direction = "e";
string row = "1";
string col = "1";
string goal_row = "-1";
string goal_col = "-1";
string serial_port = "/dev/ttyUSB0";
string initial_sensor = "35";
string velocity = "200";
string robot_mode = "f";
string algo = "astar";
int robot_port = 16;
bool v_read = false;
bool v_sent = false;
bool simple = false;
bool gui = false;


pthread_t gui_client_comm;

 
std::string filename_flag("--grid-filename=");
size_t filename_len = filename_flag.length();

std::string mode_flag("--mode=");
size_t mode_len = mode_flag.length();

std::string ip_flag("--ip=");
size_t ip_len = ip_flag.length();

std::string id_flag("--id=");
size_t id_len = id_flag.length();


std::string direction_flag("--direction=");
size_t direction_len = direction_flag.length();

std::string row_flag("--row=");
size_t row_len = row_flag.length();

std::string col_flag("--col=");
size_t col_len = col_flag.length();

std::string goal_row_flag("--goal-row=");
size_t goal_row_len = goal_row_flag.length();

std::string goal_col_flag("--goal-col=");
size_t goal_col_len = goal_col_flag.length();

std::string serial_port_flag("--serial-port=");
size_t serial_port_len = serial_port_flag.length();

std::string initial_sensor_flag("--initial-sensor=");
size_t initial_sensor_len = initial_sensor_flag.length();

std::string velocity_flag("--velocity=");
size_t velocity_len = velocity_flag.length();

std::string robot_mode_flag("--robot-mode=");
size_t robot_mode_len = robot_mode_flag.length();

std::string algo_flag("--algo=");
size_t algo_len = algo_flag.length();

std::string count_flag("--count=");
size_t count_len = count_flag.length();

std::string view_sent_messages_flag("--view-sent-messages");
size_t view_sent_messages_len = view_sent_messages_flag.length();

std::string view_read_messages_flag("--view-read-messages");
size_t view_read_messages_len = view_read_messages_flag.length();

std::string simple_flag("--simple");
size_t simple_len = simple_flag.length();

std::string gui_flag("--gui");
size_t gui_len = gui_flag.length();


void gui_client_comm_cb(void* v) {
    UdpClient* uc = (UdpClient*)v;
    uc->communicate();
}

/*Grab the command line arguments and set global vars*/
void get_command_line_args(int count, char** args) {

    for(int i=1;i<count;i++) {

        string temp(args[i]);

        if(temp.substr(0, mode_len) == mode_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            mode = temp.substr(mode_len, temp.length()-mode_len);
            //cout<<"\nmode:"<<mode;
        }

        else if(temp.substr(0, filename_len) == filename_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            if(temp[filename_len] == '/')
                grid_filename = temp.substr(filename_len, temp.length()-filename_len);
            else {
                rest = temp.substr(filename_len, temp.length()-filename_len);
                grid_filename += rest;
            }
            //cout<<"\ngrid_filename:"<<grid_filename;
        }

        else if(temp.substr(0, ip_len) == ip_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            ip = temp.substr(ip_len, temp.length()-ip_len);
            //cout<<"\nip:"<<ip;
        }

        else if(temp.substr(0, id_len) == id_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            id = temp.substr(id_len, temp.length()-id_len);
            //cout<<"\nid:"<<id;
        }

        else if(temp.substr(0, direction_len) == direction_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            direction = temp.substr(direction_len, temp.length()-direction_len);
            //cout<<"\ndirection:"<<direction;
        }


        else if(temp.substr(0, row_len) == row_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            row = temp.substr(row_len, temp.length()-row_len);
            //cout<<"\nrow:"<<row;
        }

        else if(temp.substr(0, col_len) == col_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            col = temp.substr(col_len, temp.length()-col_len);
            //cout<<"\ncol:"<<col;
        }

        else if(temp.substr(0, goal_row_len) == goal_row_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            goal_row = temp.substr(goal_row_len, temp.length()-goal_row_len);
            //cout<<"\ngoal_row:"<<goal_row;
        }

        else if(temp.substr(0, goal_col_len) == goal_col_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            goal_col = temp.substr(goal_col_len, temp.length()-goal_col_len);
            //cout<<"\ngoal_col:"<<goal_col;
        }

        else if(temp.substr(0, serial_port_len) == serial_port_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            serial_port = temp.substr(serial_port_len, temp.length()-serial_port_len);
            //=cout<<"\nserial_port:"<<serial_port;
        }

        else if(temp.substr(0, initial_sensor_len) == initial_sensor_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            initial_sensor = temp.substr(initial_sensor_len, temp.length()-initial_sensor_len);
        }

        else if(temp.substr(0, velocity_len) == velocity_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            velocity = temp.substr(velocity_len, temp.length()-velocity_len);
        }

        else if(temp.substr(0, robot_mode_len) == robot_mode_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            robot_mode = temp.substr(robot_mode_len, temp.length()-robot_mode_len);
        }

        else if(temp.substr(0, algo_len) == algo_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            algo = temp.substr(algo_len, temp.length()-algo_len);
        }

        else if(temp.substr(0, count_len) == count_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            client_count = temp.substr(count_len, temp.length()-count_len);
        }

        else if(temp == view_sent_messages_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            v_sent = true;
        }

        else if(temp == view_read_messages_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            v_read = true;
        }
        else if(temp == simple_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            simple = true;
        }
        else if(temp == gui_flag) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            gui = true;
        }
    }   //end for
}   //END GETCOMMANDLINEARGS


int main(int argc, char* args[]) {

    /*
    std::cout<<"\nargc: "<<argc;
    for(int i=0;i<argc;i++)
        cout<<"\nargs["<<i<<"]: "<<args[i];
    cout<<"\n";
    */

    if(argc < 2) {
        cout<<"\n\nError! No command line arguments specified! See ./go --help for information!\n\n";
        exit(1);
    }

    //if --help, print message and stop
    if(strcmp(args[1],"--help") == 0) {
        cout<<"\n\n\tuser options";
        cout<<"\n\t\t--help\t\t\t\tshow this message";
        cout<<"\n\t\t--mode=s|c|o\t\t\ts=server, c=client, o=own (no networking)";
        cout<<"\n\t\t--grid-filename=filename\tfilename to the grid";
        cout<<"\n\t\t--serial-port=/dev/ttyUSBx\twhich port to use for serial communication";
        cout<<"\n\t\t--ip-addr=address\t\tip address to use";
        cout<<"\n\t\t--id=any_char\t\t\tclient only - id of the client";
        cout<<"\n\t\t--direction=n|s|e|w\t\tinitial direction of the robot (defaults to e)";
        cout<<"\n\t\t--row=start_row-coordinate\tclient and only - sets starting row value";
        cout<<"\n\t\t--col=start_col-coordinate\tclient and only - sets starting col value";
        cout<<"\n\t\t--view-sent-messages\t\tshows message whenever a packet is successfully sent";
        cout<<"\n\t\t--view-read-messages\t\tshows message whenever a packet is successfully read\n";


        cout<<"\n\nHow to use";
        cout<<"\nwhen the program starts, it will prompt you for how many clients will be connecting.";
        cout<<"Once all the clients have connected, the interface will begin.";
        cout<<" At the top is the grid. The first set of lines contains information about the robot.";
        cout<<"\n\n\t1) Which robot information you are viewing.\n\t\ti.e. \"Robot 1 Information\"";
        cout<<"\n\t2) The robot's current path\n\t\ti.e. \"Path: (1,1) (1,2) (1,3)\"";
        cout<<"\n\t3) The robot's velocity in millimeters per second.\n\t\ti.e. \"Velocity: 200\"";
        cout<<"\n\t4) The robot's direction\n\t\ti.e. \"Direction: e\"";
        cout<<"\n\t5) The robot's sensor information\n\t\ti.e. \"Sensor Packet 7 high byte: -1 low byte: 3\"";
        cout<<"\n\n\tAll Together - \n\t\tRobot 1 Information\t\n\t\tPath: (1,1) (1,2) (1,3)\n\t\tVelocity: 200\n\t\tDirection: e\n\t\tSensor Packet 7 high byte: -1 low byte: 3";


        cout<<"\n\nThe next set of lines are commands and the instructions on how to input them.";
        cout<<"\n\n\t1) Change View - r ROBOT-ID";
        cout<<"\n\t\tThis command changes which robot you see information on. It takes in a robot id.\n\t\tIf I wanted to view the robot with id 1's information, I would input \"r 1\".";
        cout<<"\n\t2) Change Goal - ROBOT-ID g ROW COLUMN";
        cout<<"\n\t\tThis command changes a robot's goal. It takes in a robot id, row, and column.\n\t\tIf I wanted to change robot 2's goal to (3,5), I would input \"2 g 3 5\".";
        cout<<"\n\t3) Change Sensor Packet - ROBOT-ID s PACKET-ID";
        cout<<"\n\t\tThis command changes which sensor packet value is displayed for a robot.\n\t\tIt takes in a robot id and a sensor packet id.\n\t\tIf I wanted to view \n\t\trobot 1's bump sensor(packet-id 7), I would input \"1 s 7\".";
        cout<<"\n\t4) Change Velocity - ROBOT-ID v VELOCITY";
        cout<<"\n\t\tThis command changes the specified robot's velocity.\n\t\tIt takes in a robot id and a integer value for the velocity. Velocity is measured in\n\t\tmillimeters per second and can range from -500-500.\n\t\tIf I wanted to change robot 1's velocity to 500 mm/s, I would input \"1 v 500\"";
        cout<<"\n\t5) Change Direction - ROBOT-ID d DIRECTION";
        cout<<"\n\t\tThis command changes the specified robot's direction.\n\t\tIt takes in a robot id and a character for the direction.\n\t\tValid direction values are n,N,s,S,e,E,w,W.\n\t\tIf I wanted to change robot a's direction to n(north), I would input \"a d n\"";
        cout<<"\n\t5) Toggle Sensor Streaming - ROBOT-ID t";
        cout<<"\n\t\tThis command will toggle a robot's sensor stream. \n\t\tIf I wanted to toggle robot a's sensor streaming, input \"a t\".";
        cout<<"\n\t6) Spin - ROBOT-ID p";
        cout<<"\n\t\tThis command spins a robot. To spin robot 1, input \"1 p\".";
        cout<<"\n\t7) Quit - q";
        cout<<"\n\t\tThis command quits the program.";
        cout<<"\n\n\tChange View - v ROBOT-ID\n\tChange Goal - ROBOT-ID g ROW COLUMN\n\tChange Sensor Packet - ROBOT-ID s PACKET-ID";
        cout<<"\n\tChange Velocity - ROBOT-ID v VELOCITY\n\tChange Direction - ROBOT-ID d DIRECTION\n\tToggle Sensor Streaming - ROBOT-ID t\n\tSpin - ROBOT-ID p\n\tQuit - q\n";

        cout<<"\n\n(run --help at 100 window length for neatness)\n";
    }   //end if --help



    //else
    else {
        //get command line args
        get_command_line_args(argc, args);


        //check for serial port argument and set robot_port
        if(serial_port == "/dev/ttyUSB1")
            robot_port = 17;
        else if(serial_port == "/dev/ttyUSB2")
            robot_port = 18;
        else if(serial_port == "/dev/ttyUSB3")
            robot_port = 19;
        else if(serial_port == "/dev/ttyUSB4")
            robot_port = 20;
        else if(serial_port == "/dev/ttyUSB5")
            robot_port = 21;
        else
            robot_port = 16;

        if(gui) {

                if(mode[0] == 's') {
                    TcpServer server((char*)PORT, ip.c_str(), 1);
                    if(server.launchServer()) {
                        //set size
                        int size = server.get_num_clients();
                        //make robot
                        Robot robot(robot_port, ROBOT_BAUDRATE, '1', false);
                        //stream sensors
                        robot.streamSensors();

                        //make an agent
                        Agent a_temp(robot, direction[0]);

                        //make vector and push on the agent
                        vector<Agent> agents;
                        agents.push_back(a_temp);

                        //set the server's agents
                        server.set_agents(agents);

                        //have to set the ID because recv_init_info never gets called!!!!!!!
                        server.get_clients()[0].id = '1';

                        //make udpserver and launch
                        UdpServer u_server((char*)PORT, ip.c_str(), server.get_clients(), size);
                        if(u_server.launch_server()) {
                            GUIWindow window(a_temp, server, u_server);
                            return Fl::run();
                        }   //end if udp launch
                    }   //end if tcp launch
                }   //end if server

                else if(mode[0] == 'c') {

                    TcpClient client((char*)PORT, ip.c_str(), id[0], true);
                    UdpClient u_client((char*)PORT, ip.c_str(), id[0]);

                    Robot robot(robot_port, ROBOT_BAUDRATE, '1', 17, 19200);
                    robot.streamSensors();

                    //make agent
                    Agent* agent = new Agent(robot, 1);
                    int cs=35;
                    int a=1;
                    agent->setCurrentSensor(cs);
                    agent->getRobot()->setDefaultVelocity(0);
                    agent->set_algorithm(a);

                    client.setAgent(agent);
                    u_client.setAgent(agent);

                    if(client.launchClient() && u_client.launch_client()) {
                        pthread_create(&gui_client_comm, 0, gui_client_comm_cb, (void*)&u_client);

                        //get commands until done
                        while(!client.getDone())
                            client.get_gui_command();
                        //set udp client to true
                        u_client.setDone(true);

                        if(pthread_detach(gui_client_comm) != 0)
                            printf("\ndetach on gui_client_comm thread failed with error %m", errno);
                    }   //end if launch
                    delete agent;
                    agent = 0;
                }   //end if client

                //else, no networking needed
                else {
                    Robot robot(robot_port, ROBOT_BAUDRATE, direction[0], COMPASS_PORT, COMPASS_BAUDRATE);
                    Agent a_temp(robot, direction[0]);
                    GUIWindow window(a_temp);
                    return Fl::run();
                }   //end else
        }   //end if --gui


        //if server
        else if(mode[0] == 's') {

            //create grid
            Grid* grid = new Grid(grid_filename);
            Grid_Analyzer* grid_analyzer = new Grid_Analyzer(grid);

            //cout<<"\nTHE GRID OBJECT:\n"<<grid->toString()<<"\n\n";
            //cout<<"\nTHE GRID_ANALYZER GRID MEMBER:\n"<<grid_analyzer->getGrid()->toString()<<"\n\n";

            //get how many clients to accept
            int num_clients = atoi(client_count.c_str());
            if(num_clients < 1) {
                printf("\nError! Number of clients specified is less than one! Exiting...\n");
                exit(1);
            }
            //cout<<"\nHow many clients should be accepted?\n";
            //cin>>num_clients;

            TcpServer server((char*)PORT, ip.c_str(), num_clients, grid_filename);

            //launch the servers
            if(server.launchServer()) {
                int size = server.get_num_clients();
                UdpServer u_server((char*)PORT, ip.c_str(), server.get_clients(), size);

                if(u_server.launch_server()) {
                    cout<<"\nSuccessful Connection!\n";

                    //create the vector of robots and agents
                    vector<Robot> robots;
                    vector<Agent> agents;

                    //set and check current/initial sensor
                    int cs = atoi(initial_sensor.c_str());
                    //set current sensor
                    if(cs < 7 || cs > 42)
                        cs = OI_MODE;

                    int d;
                    if(direction == "n")
                        d = NORTH;
                    else if(direction == "s")
                        d = SOUTH;
                    else if(direction == "e" || direction == "E")
                        d = EAST;
                    else if(direction == "w" || direction == "W")
                        d = WEST;
                    else if(direction == "nw" || direction == "NW")
                        d = NORTHWEST;
                    else if(direction == "ne" || direction == "NE")
                        d = NORTHEAST;
                    else if(direction == "se" || direction == "SE")
                        d = SOUTHEAST;
                    else if(direction == "sw" || direction == "SW")
                        d = SOUTHWEST;
                    else
                        d = EAST;

                    //populate vector of agents
                    for(int i=0;i<server.get_num_clients();i++) {

                        //temp robot
                        Robot temp(robot_port, ROBOT_BAUDRATE, server.get_clients()[i].id, false);

                        //stream robot sensors
                        temp.streamSensors();

                        //put robot in vector
                        robots.push_back(temp);

                        //temp agent
                        Agent a_temp(grid, robots.at(i), d);
                        a_temp.setGridAnalyzer(grid_analyzer);

                        //push onto the vector
                        agents.push_back(a_temp);
                    }   //end for


                    //set the server's agents
                    server.set_agents(agents);

                    //set viewing preferences
                    server.set_read_mess(v_read);
                    server.set_sent_mess(v_sent);

                    //create a servercontrol object
                    ServerControl sc(&server, &u_server);

                    //go
                    sc.control();


                    //robot.pauseSensorStream();
                    server.setDone(true);
                    u_server.setDone(true);
                    usleep(1500000);
                }
            }   //end if successful connection
            delete grid_analyzer;
            grid_analyzer = 0;
            delete grid;
            grid = 0;
        }   //end if server

        //else if client
        else if(mode[0] == 'c') {

            /*SerialConnect compass_connect;
            compass_connect.OpenComport(17, 19200);

            unsigned char request_poll_command[4] = {0x55, 0xC1, 0x02, 0x02};
            compass_connect.SendBuf(17, request_poll_command, 4);
            unsigned char receive[2];
            read(compass_connect.handle, receive, 2);
            float r = get_signed_value_16_bit(receive[0], receive[1]);
            cout<<"\n"<<(int)receive[0]<<" "<<(int)receive[1];
            usleep(15000);
            cout<<"\nr:"<<r;
            compass_connect.SendBuf(17, request_poll_command, 4);
            read(compass_connect.handle, receive, 2);
            r = get_signed_value_16_bit(receive[0], receive[1]);
            r*=0.1;
            cout<<"\n"<<(int)receive[0]<<" "<<(int)receive[1];
            cout<<"\nr:"<<r;*/



            Robot robot(robot_port, ROBOT_BAUDRATE, id[0], COMPASS_PORT, COMPASS_BAUDRATE);
            TcpClient client((char*)PORT, ip.c_str(), id[0]);
            UdpClient u_client((char*)PORT, ip.c_str(), id[0]);


            //set and check current/initial sensor
            int cs = atoi(initial_sensor.c_str());
            //set current sensor
            if(cs < 7 || cs > 42)
                cs = OI_MODE;

            int v = atoi(velocity.c_str());
            if(v > 500)
                v = 500;
            else if(v < -500)
                v = -500;

            int d;
            if(direction == "n")
                d = NORTH;
            else if(direction == "s")
                d = SOUTH;
            else if(direction == "e" || direction == "E")
                d = EAST;
            else if(direction == "w" || direction == "W")
                d = WEST;
            else if(direction == "nw" || direction == "NW")
                d = NORTHWEST;
            else if(direction == "ne" || direction == "NE")
                d = NORTHEAST;
            else if(direction == "se" || direction == "SE")
                d = SOUTHEAST;
            else if(direction == "sw" || direction == "SW")
                d = SOUTHWEST;
            else
                d = EAST;

            int al;
            if(algo == "rrt")
                al = RRT;
            else
                al = ASTAR;

            robot.start();
            robot.streamSensors();
            robot.setDefaultVelocity(v);

            //set mode
            if(robot_mode == "f")
                robot.fullMode();
            else if(robot_mode == "s")
                robot.safeMode();

            //make agent
            Agent* agent = new Agent(robot, d);

            //can set current sensor with no further info, so set it now
            agent->setCurrentSensor(cs);
            agent->set_algorithm(al);

            //set the client's agent
            client.setAgent(agent);
            u_client.setAgent(agent);

            //launch the clients
            if(client.launchClient() && u_client.launch_client()) {

                cout<<"\nSuccessful Connection!";


                //make initial start and goal positions
                Position start(atoi(row.c_str()), atoi(col.c_str()));

                //check bounds for starting position
                if(agent->getGridAnalyzer()->positionValid(start))
                    agent->setPosition(start);

                //if not a valid position, start at top left of grid
                //assign the first valid position
                else {
                    int big_r = agent->getGrid()->getNumOfRows();
                    int big_c = agent->getGrid()->getNumOfCols();
                    for(int r=0;r<big_r;r++)
                        for(int c=0;c<big_c;c++) {
                            Position temp(r,c);
                            if(agent->getGridAnalyzer()->positionValid(temp)) {
                                agent->setPosition(temp);
                                r = big_r;
                                c = big_c;
                            }   //end if
                        }   //end inner for
                }   //end else invalid starting position

                if( (goal_row == "-1") || (goal_col == "-1") )
                    agent->setGoal(start);
                else {
                    Position end(atoi(goal_row.c_str()), atoi(goal_col.c_str()));
                    agent->setGoal(end);
                }



                //std::cout<<"\nID: "<<agent->getRobot()->getID()<<"\n";
                //std::cout<<"\nGRID: "<<agent->getGrid()->toString();

                //set viewing preferences
                client.set_read_mess(v_read);
                client.set_sent_mess(v_sent);

                ClientControl cc(&client, &u_client);

                //go
                cc.control();

                //close things
                robot.pauseSensorStream();
                client.setDone(true);
                u_client.setDone(true);
                usleep(1500000);
                delete agent;
                agent = 0;
                std::cout<<"\n";
            }   //end if successful connection
        }   //end if client

        //else if own
        else if(mode[0] == 'o') {

            //create grid
            Grid* grid = new Grid(grid_filename);
            Grid_Analyzer* grid_analyzer = new Grid_Analyzer(grid);

            Robot robot(robot_port, ROBOT_BAUDRATE, id[0], COMPASS_PORT, COMPASS_BAUDRATE);
             //stream sensors
            robot.streamSensors();
            //get mode
            if(robot_mode == "f")
                robot.fullMode();
            else if(robot_mode == "s")
                robot.safeMode();
            //get velocity
            int v = atoi(velocity.c_str());
            if(v > 500)
                v = 500;
            else if(v < -500)
                v = -500;
            robot.setDefaultVelocity(v);


            int cs = atoi(initial_sensor.c_str());
            //set current sensor
            if(cs < 7 || cs > 42)
                cs = OI_MODE;

            int al;
            if(algo == "rrt")
                al = RRT;
            else
                al = ASTAR;


            int d;
            if(direction == "n")
                d = NORTH;
            else if(direction == "s")
                d = SOUTH;
            else if(direction == "e" || direction == "E")
                d = EAST;
            else if(direction == "w" || direction == "W")
                d = WEST;
            else if(direction == "nw" || direction == "NW")
                d = NORTHWEST;
            else if(direction == "ne" || direction == "NE")
                d = NORTHEAST;
            else if(direction == "se" || direction == "SE")
                d = SOUTHEAST;
            else if(direction == "sw" || direction == "SW")
                d = SOUTHWEST;
            else
                d = EAST;



            //make agent and set members
            Agent* agent = new Agent(grid, robot, d);
            agent->setGridAnalyzer(grid_analyzer);
            agent->setCurrentSensor(cs);
            agent->set_algorithm(al);

            //make initial start and goal positions
            Position start(atoi(row.c_str()), atoi(col.c_str()));
            Position end(atoi(row.c_str()), atoi(col.c_str()));
            //cout<<"\nstart:"<<start.toString();
            //cout<<"\nend:"<<end.toString();
            agent->setPosition(start);
            agent->setGoal(end);

            owncontrol oc(agent);
            oc.control();


            robot.pauseSensorStream();
            delete agent;
            agent = 0;
            delete grid;
            grid = 0;
            usleep(1500000);
        }   //end if own
    }   //end else

    cout<<"\n\nExiting normally\n";
    return 0;
}   //END MAIN


