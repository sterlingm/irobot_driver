
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
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <string>




using namespace std;

string mode;
string grid_filename;
string ip;
string id;
string direction = "e";
string row = "1";
string col = "1";
string goal_row = "-1";
string goal_col = "-1";
string serial_port = "/dev/ttyUSB0";
string initial_sensor = "35";
string velocity = "200";
string robot_mode = "f";
string algo = "a*";
int robot_port = 16;
bool v_read = false;
bool v_sent = false;
bool simple = false;

/*Grab the command line arguments and set global vars*/
void get_command_line_args(int count, char** args) {

    for(int i=1;i<count;i++) {

        string temp(args[i]);

        if(strcmp(temp.substr(0,7).c_str(),"--mode=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            mode = temp.substr(7,strlen(temp.c_str())-7);
            //cout<<"\nmode:"<<mode;
        }

        else if(strcmp(temp.substr(0,16).c_str(), "--grid-filename=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            grid_filename = temp.substr(16,strlen(temp.c_str())-16);
            //cout<<"\ngrid_filename:"<<grid_filename;
        }

        else if(strcmp(temp.substr(0,5).c_str(), "--ip=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            ip = temp.substr(5,strlen(temp.c_str())-5);
            //cout<<"\nip:"<<ip;
        }

        else if(strcmp(temp.substr(0,5).c_str(), "--id=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            id = temp.substr(5, strlen(temp.c_str())-5);
            //cout<<"\nid:"<<id;
        }

        else if(strcmp(temp.substr(0,12).c_str(), "--direction=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            direction = temp.substr(12, strlen(temp.c_str())-12);
            //cout<<"\ndirection:"<<direction;
        }


        else if(strcmp(temp.substr(0,6).c_str(), "--row=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            row = temp.substr(6, strlen(temp.c_str())-6);
            //cout<<"\nrow:"<<row;
        }

        else if(strcmp(temp.substr(0,6).c_str(), "--col=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            col = temp.substr(6, strlen(temp.c_str())-6);
            //cout<<"\ncol:"<<col;
        }

        else if(strcmp(temp.substr(0,11).c_str(), "--goal-row=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            goal_row = temp.substr(11, strlen(temp.c_str())-11);
            //cout<<"\ngoal_row:"<<goal_row;
        }

        else if(strcmp(temp.substr(0,11).c_str(), "--goal-col=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            goal_col = temp.substr(11, strlen(temp.c_str())-11);
            //cout<<"\ngoal_col:"<<goal_col;
        }

        else if(strcmp(temp.substr(0,14).c_str(), "--serial-port=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            serial_port = temp.substr(14, strlen(temp.c_str())-14);
            //cout<<"\nserial_port:"<<serial_port;
        }

        else if(strcmp(temp.substr(0,17).c_str(), "--initial-sensor=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            initial_sensor = temp.substr(17, strlen(temp.c_str())-17);
        }

        else if(strcmp(temp.substr(0,11).c_str(), "--velocity=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            velocity = temp.substr(11, strlen(temp.c_str())-11);
        }

        else if(strcmp(temp.substr(0,13).c_str(), "--robot-mode=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            robot_mode = temp.substr(13, strlen(temp.c_str())-13);
        }

        else if(strcmp(temp.substr(0,7).c_str(), "--algo=") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            algo = temp.substr(7, strlen(temp.c_str())-7);
        }

        else if(strcmp(temp.c_str(), "--view-sent-messages") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            v_sent = true;
        }

        else if(strcmp(temp.c_str(), "--view-read-messages") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            v_read = true;
        }
        else if(strcmp(temp.c_str(), "--simple") == 0) {
            //cout<<"\nargs["<<i<<"]:"<<args[i];
            simple = true;
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


    else if(strcmp(args[1], "--gui") == 0) {
        /*
        Fl_Window win( 300,200,"Testing" );
        win.begin();
           Fl_Button but( 10, 150, 70, 30, "Click me" );
        win.end();
        but.callback( but_cb );
        win.show();
        return Fl::run();
        */

        Robot robot(robot_port, ROBOT_BAUDRATE, direction[0]);
        Agent a_temp(robot, direction[0]);
        GUIWindow window(a_temp);
        return Fl::run();

    }   //end if --gui


    //else
    else {
        //get command line args
        get_command_line_args(argc, args);


        //check for serial port argument and set robot_port
        if(strcmp(serial_port.c_str(), "/dev/ttyUSB1") == 0)
            robot_port = 17;
        else if(strcmp(serial_port.c_str(), "/dev/ttyUSB2") == 0)
            robot_port = 18;
        else if(strcmp(serial_port.c_str(), "/dev/ttyUSB3") == 0)
            robot_port = 19;
        else if(strcmp(serial_port.c_str(), "/dev/ttyUSB4") == 0)
            robot_port = 20;
        else if(strcmp(serial_port.c_str(), "/dev/ttyUSB5") == 0)
            robot_port = 21;
        else
            robot_port = 16;


        //if server
        if(mode[0] == 's') {

            //create grid
            Grid* grid = new Grid(grid_filename);
            Grid_Analyzer* grid_analyzer = new Grid_Analyzer(grid);

            cout<<"\nTHE GRID OBJECT:\n"<<grid->toString()<<"\n\n";
            //cout<<"\nTHE GRID_ANALYZER GRID MEMBER:\n"<<grid_analyzer->getGrid()->toString()<<"\n\n";

            //get how many clients to accept
            int num_clients;
            cout<<"\nHow many clients should be accepted?\n";
            cin>>num_clients;

            TcpServer server((char*)PORT, ip.c_str(), num_clients);

            //launch the servers
            if(server.launchServer(grid_filename)) {
                int size = server.get_num_clients();
                Udpserver u_server((char*)PORT, ip.c_str(), server.get_clients(), size);

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
                        Robot temp(robot_port, ROBOT_BAUDRATE, server.get_clients()[i].id);

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
            delete grid;
            grid = 0;
        }   //end if server

        //else if client
        else if(mode[0] == 'c') {


            Robot robot(robot_port, ROBOT_BAUDRATE, id[0]);
            TcpClient client((char*)PORT, ip.c_str(), id[0]);
            Udpclient u_client((char*)PORT, ip.c_str(), id[0]);

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
            if(strcmp(algo.c_str(), "rrt") == 0)
                al = RRT;
            else
                al = ASTAR;


            robot.streamSensors();
            robot.setVelocity(v);

            //set mode
            if(strcmp(robot_mode.c_str(), "f") == 0)
                robot.fullMode();
            else if(strcmp(robot_mode.c_str(), "s") == 0)
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

                if( (strcmp(goal_row.c_str(), "-1") == 0) || (strcmp(goal_col.c_str(), "-1") == 0) )
                    agent->setGoal(start);
                else {
                    Position end(atoi(goal_row.c_str()), atoi(goal_col.c_str()));
                    agent->setGoal(end);
                }


                Path p;
                if(al == RRT)
                    p = agent->getGridAnalyzer()->rrt_path(agent->getPosition(), agent->getGoal());
                else
                    p = agent->getGridAnalyzer()->astar_path(agent->getPosition(), agent->getGoal());
                agent->setPath(p);

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

            Robot robot(robot_port, ROBOT_BAUDRATE, id[0]);
             //set robot members and stream sensors
            robot.fullMode();
            robot.setVelocity(200);
            robot.streamSensors();

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

            //make agent and set robot's agent
            Agent* agent = new Agent(grid, robot, d);

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
        }   //end if own
    }   //end else

    cout<<"\n\nExiting normally\n";
    return 0;
}   //END MAIN


