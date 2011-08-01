
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




#define ROBOT_PORT 16
#define ROBOT_BAUDRATE 57600
#define VELOCITY 100



Robot robot(ROBOT_PORT, ROBOT_BAUDRATE);
ServerControl sc;
ClientControl cc;
TcpServer server((char*)PORT);
TcpClient client((char*)PORT);



int main(int argc, char* args[]) {
    //std::cout<<"\nargc: "<<argc;
    //std::cout<<"\nargs[0]: "<<args[1]<<"\n";

    if(args[1][0] == 's') {


        if(server.launchServer()) {


            cout<<"\nSuccessful Connection!";


            std::string file = "/home/sterling/irobot_driver/grid1.txt";
            //cout<<"\nEnter a path to a grid\n";
            //cin>>file;
            //cout<<endl<<file;
            Grid* g = new Grid(file);

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

            sc.setServer(&server);


            sc.control();

            robot.pauseSensorStream();

        }   //end if successful connection
    }   //end if server

    else if(args[1][0] == 'c') {
        if(client.launchClient()) {


            cout<<"\nSuccessful Connection!";


            std::string file = "/home/sterling/irobot_driver/labgrid.txt";
            //cout<<"\nEnter a path to a grid\n";
            //cin>>file;
            //cout<<endl<<file;
            Grid* g = new Grid(file);

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

            cc.setClient(&client);

            cc.control();

            robot.pauseSensorStream();

        }   //end if successful connection
    }   //end if client
    return 0;
}   //END MAIN


