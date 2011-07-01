
#include "utility.h"
#include "grid.h"
#include "GUIWindow.h"
#include "Robot.h"
#include "agent.h"
#include "tree.h"

#ifndef __linux__
#define sleep Sleep
#endif

using namespace std;

int main(int argc, char* args[]) {

    Robot robot(17, 57600);
    robot.fullMode();
    robot.setVelocity(100);


    std::string file;
    cout<<"\nEnter a path to a grid\n";
    cin>>file;
    cout<<endl<<file;
    Grid* g = new Grid(file);
    cout<<endl<<g->toString();

    Position start(1,1);
    Position end(1,8);

    Agent a(g, start, 'e', robot);


    Path p = a.traverse(start, end, true, false);

    a.getGrid()->markPath(p);
    cout<<"\nPath: "<<p.toString();
    cout<<a.getGrid()->toString();

    robot.stepPath(p, a.getDirection());

    //cout<<endl<<p.toString();
    //cout<<endl<<g->toString();



/*
    Agent a(g, *(new Position(3,1)), 'e');
    //set starting point on grid
    g->setPos(a.getPosition().getRow(),a.getPosition().getCol(), 'X');

    //set robot start velocity
    robot.setVelocity(175);

    int commands[] = {FORWARD, TURNCCW, FORWARD, TURNCW, FORWARD, TURNCCW, FORWARD, TURNCW, FORWARD, FORWARD, TURNCW, FORWARD, FORWARD, TURNCCW, FORWARD, FORWARD,
                        TURNCCW, FORWARD, FORWARD, TURNCW, FORWARD};
    //get the size of the command sequence
    int seqlength = sizeof(commands) / sizeof(int);

    //print grid and sleep
    cout<<endl<<a.getGrid()->toString();
    sleep(1);

    int i=0;    //counter
    bool stop=false;    //bool flag

    //solve grid and perform commands
    while( (!stop) && (i<seqlength) ) {
        sleep(1);
        if(a.processCommand(commands[i])) {
            cout<<endl<<a.getGrid()->toString();
            robot.performCommand(commands[i]);
        }
        else {
            cout<<"\nCommand "<<commands[i]<<" could not be performed!";
            stop = true;
        }
        i++;
    }   //end while
*/


    //GUIWindow window(robot);

    //return Fl::run();
    return 0;
}   //END MAIN
