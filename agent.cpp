#include <stdlib.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <iostream>
#include <stdexcept>
#include "agent.h"
#include "utility.h"

/*
 *Exception for when a path cannot be found in a grid
 */
const char* Agent::NoPathException::what() const throw() {
	return "A path cannot be found!\n";
}


/*Constructor, sets grid to g, robot to r, direction to d*/
Agent::Agent(Grid*& g, Robot& r, char d) : grid(g), robot(&r), direction(d) {}

/*Destructor*/
Agent::~Agent() {}

/*Sets direction to d if a valid direction*/
void Agent::setDirection(char d) {
    if( (d == 'n' || d == 'N') || (d == 's' || d == 'S') || (d == 'e' || d == 'E') || (d == 'w' || d == 'W') )
        direction = d;
}



void Agent::setGrid(Grid*& g) {grid = g;}
/*Sets pos to p*/
void Agent::setPosition(Position& p) {pos = p;}
/*Sets goal to g*/
void Agent::setGoal(Position& g) {goal = g;}
/*Sets path to p*/
void Agent::setPath(Path& p) {path = p;}
/*Sets robot to r*/
void Agent::setRobot(Robot& r) {robot = &r;}
/*Sets highsv to v*/
void Agent::setHighSV(int& v) {highsv = v;}
/*Sets lowsv to v*/
void Agent::setLowSV(int& v) {lowsv = v;}

/*Returns pos*/
Position& Agent::getPosition() {return pos;}
/*Returns grid*/
Grid*& Agent::getGrid() {return grid;}
/*Returns direction*/
char& Agent::getDirection() {return direction;}
/*Returns goal*/
Position& Agent::getGoal() {return goal;}
/*Returns robot*/
Robot*& Agent::getRobot() {return robot;}
/*Returns path*/
Path& Agent::getPath() {return path;}
/*Returns highsv*/
int Agent::getHighSV() {return highsv;}
/*Returns lowsv*/
int Agent::getLowSV() {return lowsv;}


/*Returns if a position is in bounds, unvisited, and accessible*/
bool Agent::positionValid(Position& p) {
    if( (p.getRow() < grid->getNumOfRows()) && (p.getCol() < grid->getNumOfCols()) &&  ((grid->getPos(p.getRow(), p.getCol()) == ' ')) )
        return true;
    return false;
}   //END POSITIONVALID


/*Returns the straight line distance between two positions*/
double Agent::getSLDistance(Position& a, Position& b) {
    return sqrt( pow(a.getRow()-b.getRow(), 2) + pow(a.getCol()-b.getCol(),2) );
}   //END GETSLDISTANCE

/*Returns a vector of the valid positions adjacent to given position*/
std::vector<Position> Agent::adjacentPositions(Position& p) {
    std::vector<Position> result;
    int startr, endr, startc, endc;

    if( (p.getRow() == 0) && (p.getCol() == 0) ) {
        startr = 0;
        startc = 0;
        endr = 1;
        endc = 1;
    }
    else if( (p.getRow() == grid->getNumOfRows()) && (p.getCol() == grid->getNumOfCols()) ) {
        startr = grid->getNumOfRows()-1;
        endr = grid->getNumOfRows();
        startc = grid->getNumOfCols()-1;
        endc = grid->getNumOfCols();
    }
    else if(p.getRow() == 0) {
        startr = 0;
        endr = 1;
        startc = p.getCol()-1;
        endc = p.getCol()+1;
    }
    else if(p.getCol() == 0) {
        startr = p.getRow()-1;
        endr = p.getRow()+1;
        startc = 0;
        endc = 1;
    }
    else if(p.getRow() == grid->getNumOfRows()) {
        startr = grid->getNumOfRows()-1;
        endr = grid->getNumOfRows();
        startc = p.getCol()-1;
        endc = p.getCol()+1;
    }
    else if(p.getCol() == grid->getNumOfCols()) {
        startr = p.getRow()-1;
        endr = p.getRow()+1;
        startc = grid->getNumOfCols()-1;
        endc = grid->getNumOfCols();
    }
    else {
        startr = p.getRow()-1;
        endr = p.getRow()+1;
        startc = p.getCol()-1;
        endc = p.getCol()+1;
    }


    for(int r=startr;r<=endr;r++)
        for(int c=startc;c<=endc;c++)
            //xor for 4-connectivity and not checking p
            if( (p.getRow() == r) ^ (p.getCol() == c) ) {
                Position temp(r,c);
                if(positionValid(temp))
                    result.push_back(temp);
            }   //end if


    return result;
}   //END GETADJACENTPOSITIONS


/*
  Returns a path from pos to end
  Throws exception if a path cannot be found
*/
Path Agent::traverse(Position& end) {

    //std::cout<<"\nTraversing from "<<pos.toString()<<" to "<<end.toString();
    //make tree and add the start position as the root node
    Tree* tree = new Tree(pos);

    //set root node values
    tree->getRoot()->setGValue(0);
    tree->getRoot()->setHValue(getSLDistance(pos,end));
    tree->getRoot()->setFValue();
    tree->getRoot()->setParent(tree->getRoot());

    //make priority queue of todo nodes and add root
    PriorityQueue nodes;
    nodes.push(tree->getRoot());

    //make path to return
    Path result;

    bool done = false;

    Tree::Node* parent = tree->getRoot();    //the parent to current for robot tracing
    Tree::Node* current = tree->getRoot();  //best node
    //**grid->setPos(current->getValue().getRow(), current->getValue().getCol(), 'x');  //set initial spot on grid

    //start traversing
    while(!done) {

        //if the queue is empty, there is no path
        if(nodes.isEmpty())
            throw new NoPathException();

        parent = current;


        //else, set grid and set current to best node
        //**grid->setPos(current->getValue().getRow(), current->getValue().getCol(), ' ');
        current = nodes.top();
        //**grid->setPos(current->getValue().getRow(), current->getValue().getCol(), 'x');
        //**std::cout<<grid->toString();

        //if agreed to best move, pop off node
        if(current == nodes.top())
            nodes.pop();

        //if we are at the goal, end loop
        if(current->getValue().equals(end))
            done = true;


        //else, traverse further
        else {

            //get successor nodes for current
            std::vector<Position> successors = adjacentPositions(current->getValue());
            //for each successor
            for(int i=0;i<successors.size();i++) {

                Tree::Node* temp = new Tree::Node(successors.at(i), current);
                //set the g value to current.gvalue + step cost (step cost is always 1)
                temp->setGValue(current->getGValue() + 1);
                //set h value
                temp->setHValue(getSLDistance(temp->getValue(), end));
                //set f value
                temp->setFValue();

                //if not equal to an ancestor node
                if(!tree->contains(successors.at(i))) {
                    //make a node
                    Tree::Node* t = tree->add(temp->getValue(), current);
                    //set function values
                    t->setGValue(temp->getGValue());
                    t->setHValue(temp->getHValue());
                    t->setFValue();
                    //push into queue
                    nodes.push(t);
                }   //end if successor not an ancestor node
            }   //end for
        }   //end else not at goal
    }   //end while


    //TRACE BACK UP THE NODES TO GET THE PATH
    while(!(current->getValue().equals(tree->getRoot()->getValue()))) {
        result.add(current->getValue());
        current = current->getParent();
    }

    //push root position
    result.add(current->getValue());


    result.reverse();
    return result;
}   //END TRAVERSE



/*Steps the robot through the path*/
void Agent::stepPath(bool own) {

<<<<<<< HEAD
void Agent::stepPath(bool own) {
        std::cout<<"\nbefore while";
        //lock path
        pthread_mutex_lock(&UTILITY_H::mutex_agent);
        std::cout<<"\nAGENT'S PATH BEFORE WHILE: "<<path.toString();
    while( path.getSize() > 1 && goal.equals(path.getPath().at(path.getSize()-1))  ) {
        std::cout<<"\ninwhile";
        //unlock
        pthread_mutex_unlock(&UTILITY_H::mutex_agent);
        //if not using server/client
=======
    try {

    //while the path contains more than 1 position AND the goal is equal to the agent's goal, move the robot
    while( path.getSize() > 1 && goal.equals(path.getPath().at(path.getSize()-1))  ) {
        //if not using server/client, print the grid
>>>>>>> ea404a9f386303bbadd3b207982390732e17ae36
        if(own) {
            grid->markPath(path);
            std::cout<<grid->toString();
        }   //end if own

        //lock path
<<<<<<< HEAD
        pthread_mutex_trylock(&UTILITY_H::mutex_agent);

        try {


            //set new position and step through first pair
            pos = path.getPath().at(1);
            robot->step(path.getPath().at(0), path.getPath().at(1), direction);

            //delete the first position
            path.getPath().erase(path.getPath().begin());

=======
        pthread_mutex_lock(&mutex_agent);


        //set new position and step through first pair
        pos = path.getPath().at(1);
        robot->step(path.getPath().at(0), path.getPath().at(1), direction);
>>>>>>> ea404a9f386303bbadd3b207982390732e17ae36

        } catch(std::out_of_range& e) {}


        //unlock
        pthread_mutex_unlock(&mutex_agent);
    }   //end while

    } catch(std::out_of_range& e) {}
}   //END STEPPATH


