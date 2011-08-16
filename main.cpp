
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




#ifndef __linux__
#define sleep Sleep
#endif




using namespace std;




Robot robot(ROBOT_PORT, ROBOT_BAUDRATE);
ServerControl sc;
ClientControl cc;
TcpServer server((char*)PORT);
TcpClient client((char*)PORT);
udpserver u_server((char*)UDPPORT);
udpclient u_client((char*)UDPPORT);



int main(int argc, char* args[]) {
    /*
    std::cout<<"\nargc: "<<argc;
    for(int i=0;i<argc;i++)
        cout<<"\nargs["<<i<<"]: "<<args[i];
    cout<<"\n";
    */

    if(args[1][0] == 's') {


        if(server.launchServer() && u_server.launch_server()) {


            cout<<"\nSuccessful Connection!";

            std::stringstream temp_file;
            temp_file<<"./"<<args[2];

            std::string filename = temp_file.str();
            //cout<<"\nEnter a path to a grid\n";
            //cin>>file;
            cout<<endl<<filename;
            Grid* g = new Grid(filename);

            Agent* agent = new Agent(g, robot, 'e');


            cout<<endl<<g->toString();

            robot.setAgent(agent);
            robot.fullMode();
            robot.setVelocity(VELOCITY);


            Position start(1,1);
            Position end(1,1);
            agent->setPosition(start);
            agent->setGoal(end);


            Position goal = agent->getGoal();
            Path p = agent->traverse(goal);
            agent->setPath(p);

            server.setAgent(agent);
            u_server.setAgent(agent);

            sc.setServer(&server);
            sc.setUDP(&u_server);


            sc.control();

            robot.pauseSensorStream();

        }   //end if successful connection
    }   //end if server

    else if(args[1][0] == 'c' && u_client.launch_client()) {
        if(client.launchClient()) {


            cout<<"\nSuccessful Connection!";

            std::stringstream temp_file;
            temp_file<<"./"<<args[2];

            std::string filename = temp_file.str();
            //cout<<"\nEnter a path to a grid\n";
            //cin>>file;
            cout<<endl<<filename;
            Grid* g = new Grid(filename);

            Agent* agent = new Agent(g, robot, 'e');


            cout<<endl<<g->toString();

            robot.setAgent(agent);
            robot.fullMode();
            robot.setVelocity(VELOCITY);

            agent->getRobot()->streamSensors();


            Position start(1,1);
            Position end(1,1);
            agent->setPosition(start);
            agent->setGoal(end);


            Position goal = agent->getGoal();
            Path p = agent->traverse(goal);
            agent->setPath(p);

            client.setAgent(agent);
            u_client.setAgent(agent);

            cc.setClient(&client);
            cc.setUDP(&u_client);

            cc.control();

            robot.pauseSensorStream();

        }   //end if successful connection
    }   //end if client

    cout<<"\n\nExiting normally\n";
    return 0;
}   //END MAIN


