#include <stdlib.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <stdexcept>
#include "robot_driver_agent.h"
#include "robot_driver_utility.h"

/*
 *Exception for when a path cannot be found in a grid
 */
const char* Agent::NoPathException::what() const throw() {
	return "A path cannot be found!\n";
}

Agent::Agent() : grid(0), robot(0), spinning(false), driving(0), ga(0) {}


Agent::Agent(Robot& r) : grid(0), robot(&r), direction(0), highsv(-1), lowsv(-1), spinning(false), driving(0), ga(0) {}

/*Constructor, sets grid to g, robot to r, direction to d*/
Agent::Agent(Robot& r, int d) : grid(0), robot(&r), direction(d), highsv(-1), lowsv(-1), spinning(false), driving(0), ga(0) {}

/*Constructor, sets grid to g, robot to r, direction to d*/
Agent::Agent(Grid*& g, Robot& r, int d) : grid(g), robot(&r), direction(d), highsv(-1), lowsv(-1), spinning(false), driving(0), ga(0) {}

/*Destructor*/
Agent::~Agent() {}

/*Sets direction to d if a valid direction*/
void Agent::setDirection(int d) {
    pthread_mutex_lock(&mutex_agent);
    direction = d;
    pthread_mutex_unlock(&mutex_agent);
}
/*Sets pos to p*/
void Agent::setPosition(Position& p) {
    pthread_mutex_lock(&mutex_agent);
    pos = p;
    pthread_mutex_unlock(&mutex_agent);
}
/*Sets goal to g*/
void Agent::setGoal(Position& g) {
    //pthread_mutex_lock(&mutex_agent);
    goal = g;
    //pthread_mutex_unlock(&mutex_agent);
}
/*Sets path to p*/
void Agent::setPath(Path& p) {
    pthread_mutex_lock(&mutex_agent);
    path = p;
    pthread_mutex_unlock(&mutex_agent);
}
/*Sets highsv to v*/
void Agent::setHighSV(int& v) {highsv = v;}
/*Sets lowsv to v*/
void Agent::setLowSV(int& v) {lowsv = v;}
/*Sets spinning to s*/
void Agent::set_spinning(bool s) {spinning = s;}
/*Sets driving to d*/
void Agent::set_driving(bool d) {driving = d;}
/*Sets grid to g*/
void Agent::setGrid(Grid* g) {grid = g;}

void Agent::setGridAnalyzer(Grid_Analyzer* grida) {ga = grida;}
/*Sets the current sensor to s*/
void Agent::setCurrentSensor(int& s) {currentSensor = s;}
/*Sets the mode to m*/
void Agent::set_mode(char& m) {mode = m;}

void Agent::set_algorithm(int& a) {algorithm = a;}

/*Returns pos*/
Position& Agent::getPosition() {return pos;}
/*Returns grid*/
Grid*& Agent::getGrid() {return grid;}

Grid_Analyzer*& Agent::getGridAnalyzer() {return ga;}
/*Returns direction*/
int& Agent::getDirection() {return direction;}
/*Returns goal*/
Position& Agent::getGoal() {return goal;}
/*Returns robot*/
Robot*& Agent::getRobot() {return robot;}
/*Returns path*/
Path& Agent::getPath() {return path;}
/*Returns highsv*/
int& Agent::getHighSV() {return highsv;}
/*Returns lowsv*/
int& Agent::getLowSV() {return lowsv;}
/*Returns mode*/
char& Agent::get_mode() {return mode;}
/*Returns the current sensor*/
int& Agent::getCurrentSensor() {return currentSensor;}
/*Returns spinning*/
bool Agent::is_spinning() {return spinning;}
/*Returns driving*/
bool Agent::is_driving() {return driving;}

int Agent::get_algorithm() {return algorithm;}





void Agent::change_direction(int after) {

    if(after == EAST || after == NORTH || after == WEST || after == SOUTH ||
       after == NORTHEAST || after == NORTHWEST || after == SOUTHEAST || after == SOUTHWEST) {

        //if facing east
        if(direction == EAST) {

            if(after == NORTH)
                robot->turnXDegrees(90);
            else if(after == SOUTH)
                robot->turnXDegrees(-90);
            else if(after == WEST)
                robot->turnXDegrees(180);
            else if(after == NORTHEAST)
                robot->turnXDegrees(45);
            else if(after == NORTHWEST)
                robot->turnXDegrees(135);
            else if(after == SOUTHWEST)
                robot->turnXDegrees(-135);
            else if(after == SOUTHEAST)
                robot->turnXDegrees(-45);

        }   //end if facing east

        //if facing west
        else if(direction == WEST) {

            if(after == NORTH)
                robot->turnXDegrees(-90);
            else if(after == SOUTH)
                robot->turnXDegrees(90);
            else if(after == EAST)
                robot->turnXDegrees(180);
            else if(after == NORTHEAST)
                robot->turnXDegrees(-135);
            else if(after == NORTHWEST)
                robot->turnXDegrees(-45);
            else if(after == SOUTHWEST)
                robot->turnXDegrees(45);
            else if(after == SOUTHEAST)
                robot->turnXDegrees(135);

        }   //end if facing west

        //if facing north
        else if(direction == NORTH) {

            if(after == WEST)
                robot->turnXDegrees(90);
            else if(after == EAST)
                robot->turnXDegrees(-90);
            else if(after == SOUTH)
                robot->turnXDegrees(180);
            else if(after == NORTHEAST)
                robot->turnXDegrees(-45);
            else if(after == NORTHWEST)
                robot->turnXDegrees(45);
            else if(after == SOUTHWEST)
                robot->turnXDegrees(135);
            else if(after == SOUTHEAST)
                robot->turnXDegrees(-135);

        }   //end if facing north

        //if facing south
        else if(direction == SOUTH) {

            if(after == WEST)
                robot->turnXDegrees(-90);
            else if(after == EAST)
                robot->turnXDegrees(90);
            else if(after == NORTH)
                robot->turnXDegrees(180);
            else if(after == NORTHEAST)
                robot->turnXDegrees(135);
            else if(after == NORTHWEST)
                robot->turnXDegrees(-135);
            else if(after == SOUTHWEST)
                robot->turnXDegrees(-45);
            else if(after == SOUTHEAST)
                robot->turnXDegrees(45);

        }   //end if facing south

        else if(direction == NORTHEAST) {
            if(after == WEST)
                robot->turnXDegrees(135);
            else if(after == EAST)
                robot->turnXDegrees(-45);
            else if(after == NORTH)
                robot->turnXDegrees(45);
            else if(after == SOUTH)
                robot->turnXDegrees(-135);
            else if(after == NORTHWEST)
                robot->turnXDegrees(90);
            else if(after == SOUTHWEST)
                robot->turnXDegrees(180);
            else if(after == SOUTHEAST)
                robot->turnXDegrees(-90);
        }

        else if(direction == NORTHWEST) {
            if(after == WEST)
                robot->turnXDegrees(45);
            else if(after == EAST)
                robot->turnXDegrees(-135);
            else if(after == NORTH)
                robot->turnXDegrees(-45);
            else if(after == SOUTH)
                robot->turnXDegrees(135);
            else if(after == NORTHEAST)
                robot->turnXDegrees(-90);
            else if(after == SOUTHWEST)
                robot->turnXDegrees(90);
            else if(after == SOUTHEAST)
                robot->turnXDegrees(180);
        }

        else if(direction == SOUTHEAST) {
            if(after == WEST)
                robot->turnXDegrees(-135);
            else if(after == EAST)
                robot->turnXDegrees(45);
            else if(after == NORTH)
                robot->turnXDegrees(135);
            else if(after == SOUTH)
                robot->turnXDegrees(-45);
            else if(after == NORTHEAST)
                robot->turnXDegrees(90);
            else if(after == SOUTHWEST)
                robot->turnXDegrees(-90);
            else if(after == NORTHWEST)
                robot->turnXDegrees(180);
        }

        else if(direction == SOUTHWEST) {
            if(after == WEST)
                robot->turnXDegrees(-45);
            else if(after == EAST)
                robot->turnXDegrees(135);
            else if(after == NORTH)
                robot->turnXDegrees(-135);
            else if(after == SOUTH)
                robot->turnXDegrees(45);
            else if(after == NORTHEAST)
                robot->turnXDegrees(180);
            else if(after == SOUTHEAST)
                robot->turnXDegrees(90);
            else if(after == NORTHWEST)
                robot->turnXDegrees(-90);
        }

        direction = after;
    }
}





/*Robot moves from one position to another
  Sets the agent position to b*/
bool Agent::step(Position& a, Position& b) {

    std::cout<<"\nStepping from "<<a.toString()<<" to "<<b.toString()<<"\n";

    //if positions not connected
    if( (abs(a.getCol() - b.getCol()) > 1) || (abs(a.getRow() - b.getRow()) > 1) ) {
        std::cout<<"\nCannot step from "<<a.toString()<<" to "<<b.toString();
        return false;
    }

    //if moving horizontal
    else if(a.getRow() == b.getRow()) {

        //if moving east
        if( (a.getCol() - b.getCol() == -1) ) {

            if(robot->getVelocity() > 0)
                change_direction(EAST);
            else
                change_direction(WEST);

            robot->driveXDistance(UNIT_SIZE_STRAIGHT);
        }   //end if moving right

        //if moving west
        else if( (a.getCol() - b.getCol() == 1) ) {

            if(robot->getVelocity() > 0)
                change_direction(WEST);
            else
                change_direction(EAST);

            robot->driveXDistance(UNIT_SIZE_STRAIGHT);
        }   //end if moving west

        return true;
    }   //end if moving horizontally


    //if moving vertically
    else if(a.getCol() == b.getCol()) {

        //if moving south
        if( (a.getRow() - b.getRow() == -1) ) {

            if(robot->getVelocity() > 0)
                change_direction(SOUTH);
            else
                change_direction(NORTH);

            robot->driveXDistance(UNIT_SIZE_STRAIGHT);
        }   //end if moving south

        //if moving north
        else if( (a.getRow() - b.getRow() == 1) ) {

            if(robot->getVelocity() > 0)
                change_direction(NORTH);
            else
                change_direction(SOUTH);

            robot->driveXDistance(UNIT_SIZE_STRAIGHT);
        }   //end if moving north

        return true;
    }   //end if moving vertically

    //if moving diagonally
    else {

        //if moving northeast
        if( (a.getRow() - b.getRow() == 1) && (a.getCol() - b.getCol() == -1) ) {
            if(robot->getVelocity() > 0)
                change_direction(NORTHEAST);
            else
                change_direction(SOUTHWEST);

            robot->driveXDistance(UNIT_SIZE_DIAGONAL);
        }   //end if moving northeast

        //if moving north west
        else if( (a.getRow() - b.getRow() == 1) && (a.getCol() - b.getCol() == 1) ) {
            if(robot->getVelocity() > 0)
                change_direction(NORTHWEST);
            else
                change_direction(SOUTHEAST);

            robot->driveXDistance(UNIT_SIZE_DIAGONAL);
        }   //end if moving northwest

        //if moving southeast
        else if( (a.getRow() - b.getRow() == -1) && (a.getCol() - b.getCol() == -1) ) {
            if(robot->getVelocity() > 0)
                change_direction(SOUTHEAST);
            else
                change_direction(NORTHWEST);

            robot->driveXDistance(UNIT_SIZE_DIAGONAL);
        }   //end if moving southeast

        //if moving southwest
        else { // if( (a.getRow() - b.getRow() == -1) && (a.getCol() - b.getCol() == 1) )
            if(robot->getVelocity() > 0)
                change_direction(SOUTHWEST);
            else
                change_direction(NORTHEAST);

            robot->driveXDistance(UNIT_SIZE_DIAGONAL);
        }   //end if moving southwest

        return true;
    }   //end if moving diagonally
}   //END STEP




/*Steps the robot through the path*/
void Agent::stepPath(bool own) {

    try {

        //trylock to check in while
        pthread_mutex_trylock(&mutex_agent);

        Position init;
        Position next;
        //while there is a path
        while( path.getSize() > 1) {
            //unlock path
            pthread_mutex_unlock(&mutex_agent);
            //lock path
            pthread_mutex_trylock(&mutex_agent);

            //std::cout<<"\npath.getPathVector.at(1):"<<path.getPathVector().at(1).toString()<<" spinning:"<<spinning<<" robot velocity:"<<robot->getVelocity();
            //make sure position is still valid
            if( (!spinning) && (!driving) && (robot->getVelocity() != 0) ) {
                driving = true;

                //set new position and step through first pair
                //pos = path.getPath().at(1);
                init = path.getPathVector().at(0);
                next = path.getPathVector().at(1);

                if(step(init, next)) {
                    pos = next;
                    //delete the first position
                    path.pop_front();

                    //give time to send new position to server
                    if(algorithm == ASTAR) usleep(2000000);
                    else usleep(4000000);
                }   //end if successful step


                driving = false;
            }   //end if
            //else {
                //std::cout<<"\nspinning:"<<spinning<<" driving:"<<driving<<" robot->getVelocity():"<<robot->getVelocity();
            //}
            //unlock
            pthread_mutex_unlock(&mutex_agent);
        }   //end while
    } catch(std::out_of_range& e) {}
}   //END STEPPATH


