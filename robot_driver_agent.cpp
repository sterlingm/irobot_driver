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

Agent::Agent() : grid(0), robot(0), spinning(false), ga(0) {}


Agent::Agent(Robot& r) : grid(0), robot(&r), direction(0), highsv(-1), lowsv(-1), spinning(false), ga(0) {}

/*Constructor, sets grid to g, robot to r, direction to d*/
Agent::Agent(Robot& r, int d) : grid(0), robot(&r), direction(d), highsv(-1), lowsv(-1), spinning(false), ga(0) {}

/*Constructor, sets grid to g, robot to r, direction to d*/
Agent::Agent(Grid*& g, Robot& r, int d) : grid(g), robot(&r), direction(d), highsv(-1), lowsv(-1), spinning(false), ga(0) {}

/*Destructor*/
Agent::~Agent() {}

/*Sets direction to d if a valid direction*/
void Agent::setDirection(int d) {direction = d;}
/*Sets pos to p*/
void Agent::setPosition(Position& p) {pos = p;}
/*Sets goal to g*/
void Agent::setGoal(Position& g) {goal = g;}
/*Sets path to p*/
void Agent::setPath(Path& p) {path = p;}
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


/*Returns if a position is in bounds, unvisited, and accessible*/ /*
bool Agent::positionValid(Position& p) {
    if( (p.getRow() < grid->getNumOfRows()) && (p.getCol() < grid->getNumOfCols()) &&  ((grid->getPos(p.getRow(), p.getCol()) == ' ')) )
        return true;
    return false;
}   //END POSITIONVALID */


/*Returns the straight line distance between two positions*/ /*
double Agent::getSLDistance(Position& a, Position& b) {
    return sqrt( pow(a.getRow()-b.getRow(), 2) + pow(a.getCol()-b.getCol(),2) );
}   //END GETSLDISTANCE */

/*Returns a vector of the valid positions adjacent to given position*/ /*
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
            if( !((p.getRow() == r) && (p.getCol() == c)) ) {
                Position temp(r,c);
                if(positionValid(temp))
                    result.push_back(temp);
            }   //end if


    return result;
}   //END GETADJACENTPOSITIONS */


/*
  Returns a path from pos to end
  Throws exception if a path cannot be found
*/ /*
Path Agent::astar_path(Position& init, Position& end) {
    //std::cout<<"\nTraversing from "<<pos.toString()<<" to "<<end.toString()<<"\n";

    //make tree and add the start position as the root node
    Tree* tree = new Tree(init);

    //set root node values
    tree->getRoot()->setGValue(0);
    tree->getRoot()->setHValue(getSLDistance(pos,end));
    tree->getRoot()->setFValue();
    tree->getRoot()->setParent(tree->getRoot());

    //make priority queue of todo nodes and add the root to queue
    PriorityQueue nodes;
    nodes.push(tree->getRoot());

    //make path to return
    Path result;

    //flag to stop
    bool done = false;

    //the parent to current for robot tracing
    Tree::Node* parent = tree->getRoot();

    //best node
    Tree::Node* current = tree->getRoot();
    //**grid->setPos(current->getValue().getRow(), current->getValue().getCol(), 'x');  //set initial spot on grid

    //start traversing
    while(!done) {

        //if the queue is empty, there is no path
        if(nodes.isEmpty()) {
            //call find_next_best
            Position temp = find_next_best(*tree, end);

            //set end to the next best position
            end = temp;

            //set current to the next_best node to end the traversal
            current = tree->find(temp);

            //repush current onto queue so we don't come into this block again
            nodes.push(current);

        }   //end if queue empty

        //else there are more positions to try
        else {

            //std::cout<<"\nTREE WHEN CURRENT IS "<<current->getValue().toString()<<tree->toString()<<"\n";
            //set the new parent
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
                //std::cout<<"\nPositions adjacent to "<<current->getValue().toString();
                //for(int i=0;i<successors.size();i++)
                    //std::cout<<"\n"<<successors.at(i).toString();

                //for each successor
                for(int i=0;i<successors.size();i++) {

                    //if not equal to an ancestor node
                    if(!tree->contains(successors.at(i))) {

                        //make temp node for the tree
                        Tree::Node temp(successors.at(i), current);
                        //set the g value to current.gvalue + step cost (step cost is always 1)
                        temp.setGValue(current->getGValue() + 1);
                        //set h value
                        temp.setHValue(getSLDistance(temp.getValue(), end));
                        //set f value
                        temp.setFValue();

                        //make a node* for queue
                        Tree::Node* t = tree->add(temp.getValue(), current);
                        //set function values
                        t->setGValue(temp.getGValue());
                        t->setHValue(temp.getHValue());
                        t->setFValue();

                        //push into queue
                        nodes.push(t);

                    }   //end if successor not an ancestor node
                }   //end for
            }   //end else not at goal
        }   //end else still positions to try
    }   //end while


    //trace back up the nodes to get the path
    while(!(current->getValue().equals(tree->getRoot()->getValue()))) {
        result.add(current->getValue());
        current = current->getParent();
    }   //end while

    //push root position
    result.add(current->getValue());

    //reverse the path vector so first position is vector.at(0), last position is vector.at(vector_length-1)
    result.reverse();
    delete tree;
    //std::cout<<"\nASTAR RETURNING:"<<result.toString();
    return result;
}   //END ASTAR */


/*
Tree::Node* Agent::find_closest_node_in_tree(Tree*& tree, Position& ver) {
    Tree::Node* result = tree->getRoot();
    //loop through tree nodes, compare distance
    for(int i=0;i<tree->getNodes().size();i++) {
        if(getSLDistance(tree->getNodes().at(i)->getValue(), ver)
           < getSLDistance(result->getValue(), ver))
           result = tree->getNodes().at(i);
    }   //end for

    return result;
}   //END FIND_CLOSEST_IN_TREE */


/*Gets surrounding positions of the position to form a group to pick a random sample from*/ /*
std::vector<Position> Agent::get_potential_samples(int& rk, int& ck) {

    std::vector<Position> result;
    int startr;
    int startc;
    int endr;
    int endc;

    if(pos.getRow()-rk < 0)
        startr = 0;
    else
        startr = pos.getRow()-rk;

    if(pos.getCol()-ck < 0)
        startc = 0;
    else
        startc = pos.getCol() - ck;

    if(pos.getRow()+rk >= grid->getNumOfRows())
        endr = grid->getNumOfRows()-1;
    else
        endr = pos.getRow() + rk;

    if(pos.getCol()+ck >= grid->getNumOfCols())
        endc = grid->getNumOfCols()-1;
    else
        endc = grid->getNumOfCols()+ck;

    for(int r=startr;r<endr;r++) {
        for(int c=startc;c<endc;c++) {
            Position temp(r,c);
            result.push_back(temp);
        }
    }
    return result;
}   //END GET_POTENTIAL_SAMPLES */


/*
Path Agent::rrt_path(Position& init, Position& end) {

    //make tree with pos as the init state
    Tree* tree = new Tree(init);

    Position x = pos;
    int r_samp = 2;
    int c_samp = 2;
    srand(time(NULL));

    //while the goal is not found
    while(!tree->contains(end)) {


        /*ANY RANDOM STATE!
        //xrand<-random_state
        //make a random position
        srand(rand());
        int row = (rand() % grid->getNumOfRows())+1;
        srand(rand());
        int col = (rand() % grid->getNumOfCols())+1;
        Position rand_state(row, col);
        //std::cout<<"\nrand_state: "<<rand_state.toString();


        //get list of potential samples
        std::vector<Position> potential_samples = get_potential_samples(r_samp, c_samp);
        //seed again with rand
        srand(rand());
        //get one of the samples
        Position rand_state = potential_samples.at(rand() % potential_samples.size());
        //std::cout<<"\nrand_state: "<<rand_state.toString()<<"\n";
        //increase r and c samp
        r_samp += r_samp;
        c_samp += c_samp;
        //avoid going over int max storage
        if(r_samp >= grid->getNumOfRows()) {
            r_samp = grid->getNumOfRows()-pos.getRow()-1;
            c_samp = grid->getNumOfCols()-pos.getCol()-1;
        }

        //if position is valid and not already in tree,
        //continue with algorithm
        if(positionValid(rand_state) && !tree->contains(rand_state)) {

            //xnear<-nearest_neighbor
            Tree::Node* near = find_closest_node_in_tree(tree, rand_state);
            //std::cout<<"\nfind_closest_node_in_tree returned:"<<near->getValue().toString()<<"\n";

            //get a path from nearest point to the random point
            Path connect_points = astar_path(near->getValue(), rand_state);

            //add the path to the tree
            for(int i=1;i<connect_points.getPathVector().size();i++)
                tree->add(connect_points.getPathVector().at(i), tree->find(connect_points.getPathVector().at(i-1)));

            //std::cout<<"\ntree:\n"<<tree->toString()<<"\n\n";
        }   //end if
    }   //end while


    Path result;
    Tree::Node* current = tree->find(end);

    //trace back up the nodes to get the path
    while(!(current->getValue().equals(tree->getRoot()->getValue()))) {
        result.add(current->getValue());
        current = current->getParent();
    }

    result.add(current->getValue());
    result.reverse();

    //std::cout<<"\nrrt path from init:"<<init.toString()<<" to end:"<<end.toString()<<" :\n"<<result.toString();

    delete tree;
    return result;
}   //END RRT_PATH
 */





/*Finds the closest position in tree to position e*/ /*
Position Agent::find_next_best(Tree& tree, Position e) {

    //std::cout<<"tree in find_next_best for "<<e.toString()<<":"<<tree.toString()<<"\n";

    Position result;
    Tree::Node* nb = tree.getNodes().at(0);
    for(int i=1;i<tree.getNodes().size();i++) {

        //std::cout<<"\ncomparing "<<nb->getValue().toString()<<" with "<<tree.getNodes().at(i)->getValue().toString();
        //std::cout<<"\nnb->getHValue: "<<nb->getHValue()<<" "<<tree.getNodes().at(i)->getValue().toString()<<"->HValue: "<<tree.getNodes().at(i)->getHValue();

        //check straight line distance
        if( tree.getNodes().at(i)->getHValue() < nb->getHValue() )
            nb = tree.getNodes().at(i);

        //if straight line distance is equal, check step cost
        else if( tree.getNodes().at(i)->getHValue() == nb->getHValue() )
            if( tree.getNodes().at(i)->getGValue() < nb->getGValue() )
                nb = tree.getNodes().at(i);

    }   //end for

    result = nb->getValue();

    //std::cout<<"\nFIND NEXT BEST RETURNING: "<<result.toString();
    return result;
}*/



/*Steps the robot through the path*/
void Agent::stepPath(bool own) {

    //std::cout<<"\npath in stepPath: "<<path.toString()<<"\n";
    try {

        //trylock to check in while
        pthread_mutex_trylock(&mutex_agent);

        //while there is a path
        while( path.getSize() > 1) {

            //unlock path
            pthread_mutex_unlock(&mutex_agent);
            //lock path
            pthread_mutex_lock(&mutex_agent);

            //std::cout<<"\npath.getPathVector.at(1):"<<path.getPathVector().at(1).toString()<<" spinning:"<<spinning<<" robot velocity:"<<robot->getVelocity();
            //make sure position is still valid
            if( (!spinning) && (robot->getVelocity() != 0) ) {

                driving = true;

                //set new position and step through first pair
                //pos = path.getPath().at(1);
                step(path.getPathVector().at(0), path.getPathVector().at(1));

                //delete the first position
                path.pop_front();

                driving = false;
            }   //end if
            //unlock
            pthread_mutex_unlock(&mutex_agent);
        }   //end while
    } catch(std::out_of_range& e) {}
}   //END STEPPATH





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
void Agent::step(Position& a, Position& b) {

    //std::cout<<"\nStepping from "<<a.toString()<<" to "<<b.toString()<<"\n";

    //if positions not connected
    if( (abs(a.getCol() - b.getCol()) > 1) || (abs(a.getRow() - b.getRow()) > 1) )
        std::cout<<"\nCannot step from "<<a.toString()<<" to "<<b.toString();

    //if moving horizontal
    else if(a.getRow() == b.getRow()) {

        //if moving east
        if( (a.getCol() - b.getCol() == -1) ) {

            if(robot->getVelocity() > 0)
                change_direction(EAST);
            else
                change_direction(WEST);

            pos = b;
            robot->driveXDistance(UNIT_SIZE);
        }   //end if moving right

        //if moving west
        else if( (a.getCol() - b.getCol() == 1) ) {

            if(robot->getVelocity() > 0)
                change_direction(WEST);
            else
                change_direction(EAST);

            pos = b;
            robot->driveXDistance(UNIT_SIZE);
        }   //end if moving west
    }   //end if moving horizontally


    //if moving vertically
    else if(a.getCol() == b.getCol()) {

        //if moving south
        if( (a.getRow() - b.getRow() == -1) ) {

            if(robot->getVelocity() > 0)
                change_direction(SOUTH);
            else
                change_direction(NORTH);

            pos = b;
            robot->driveXDistance(UNIT_SIZE);
        }   //end if moving south

        //if moving north
        else if( (a.getRow() - b.getRow() == 1) ) {

            if(robot->getVelocity() > 0)
                change_direction(NORTH);
            else
                change_direction(SOUTH);
            pos = b;
            robot->driveXDistance(UNIT_SIZE);
        }   //end if moving north

        pos = b;
    }   //end if moving vertically

    //if moving diagonally
    else {

        //if moving northeast
        if( (a.getRow() - b.getRow() == 1) && (a.getCol() - b.getCol() == -1) ) {
            if(robot->getVelocity() > 0)
                change_direction(NORTHEAST);
            else
                change_direction(SOUTHWEST);

            pos = b;
            robot->driveXDistance(UNIT_SIZE);
        }   //end if moving northeast

        //if moving north west
        else if( (a.getRow() - b.getRow() == 1) && (a.getCol() - b.getCol() == 1) ) {
            if(robot->getVelocity() > 0)
                change_direction(NORTHWEST);
            else
                change_direction(SOUTHEAST);

            pos = b;
            robot->driveXDistance(UNIT_SIZE);
        }   //end if moving northwest

        //if moving southeast
        else if( (a.getRow() - b.getRow() == -1) && (a.getCol() - b.getCol() == -1) ) {
            if(robot->getVelocity() > 0)
                change_direction(SOUTHEAST);
            else
                change_direction(NORTHWEST);

            pos = b;
            robot->driveXDistance(UNIT_SIZE);
        }   //end if moving southeast

        //if moving southwest
        else { // if( (a.getRow() - b.getRow() == -1) && (a.getCol() - b.getCol() == 1) )
            if(robot->getVelocity() > 0)
                change_direction(SOUTHWEST);
            else
                change_direction(NORTHEAST);

            pos = b;
            robot->driveXDistance(UNIT_SIZE);
        }   //end if moving southwest
    }   //end if moving diagonally
}   //END STEP

