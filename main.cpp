
#include "utility.h"
#include "grid.h"
#include "GUIWindow.h"
#include "Robot.h"
#include "agent.h"
#include "clientcontrol.h"
#include "servercontrol.h"
#include "tree.h"
#include "tcpserver.h"
#include "tcpclient.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>




using namespace std;

Robot robot(ROBOT_PORT, ROBOT_BAUDRATE);
ServerControl sc;
ClientControl cc;
TcpServer server((char*)PORT);
TcpClient client((char*)PORT);
udpserver u_server((char*)PORT);
udpclient u_client((char*)PORT);



int main(int argc, char* args[]) {
    /*
    std::cout<<"\nargc: "<<argc;
    for(int i=0;i<argc;i++)
        cout<<"\nargs["<<i<<"]: "<<args[i];
    cout<<"\n";
    */


    /*initialize things*/

    //get grid filename from command line argument 2
    std::stringstream temp_file;
    temp_file<<"./"<<args[2];
    std::string filename = temp_file.str();
    //cout<<endl<<filename;
    Grid* g = new Grid(filename);
    //print grid
    cout<<endl<<g->toString();

    //set robot members and stream sensors
    robot.fullMode();
    robot.setVelocity(VELOCITY);
    robot.streamSensors();


    //make agent and set robot's agent
    Agent* agent = new Agent(g, robot, 'e');
    robot.setAgent(agent);

    //make initial start and goal positions
    Position start(1,1);
    Position end(1,1);
    agent->setPosition(start);
    agent->setGoal(end);

    //set initial path
    //Position goal = agent->getGoal();
    Path p = agent->traverse(agent->getGoal());
    agent->setPath(p);


    //set the ip addresses
    server.setIP(args[3]);
    client.setIP(args[3]);
    u_server.setIP(args[3]);
    u_client.setIP(args[3]);


    //if server
    if(args[1][0] == 's') {

        //attempt to launch the servers
        if(server.launchServer() && u_server.launch_server()) {



            cout<<"\nSuccessful Connection!";

            //set the agents
            server.setAgent(agent);
            u_server.setAgent(agent);

            //set server control's member
            sc.setServer(&server);
            sc.setUDP(&u_server);

            //go
            sc.control();

            robot.pauseSensorStream();

        }   //end if successful connection
    }   //end if server

    //else if client
    else if(args[1][0] == 'c') {


        //launch the clients
        if(client.launchClient() && u_client.launch_client()) {

            cout<<"\nSuccessful Connection!";

            //set the agents
            client.setAgent(agent);
            u_client.setAgent(agent);

            //set client control's members
            cc.setClient(&client);
            cc.setUDP(&u_client);

            //go
            cc.control();

            robot.pauseSensorStream();

        }   //end if successful connection
    }   //end if client

    //else if own
    else if(args[1][0] == 'o') {

        //step through the path
        agent->stepPath(true);

        //mark final position and print grid
        Path last = agent->getPath();
        agent->getGrid()->markPath(last);
        cout<<agent->getGrid()->toString();

        robot.pauseSensorStream();
    }   //end if own

    cout<<"\n\nExiting normally\n";
    return 0;
}   //END MAIN


