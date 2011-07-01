#include <stdlib.h>
#include <string>
#include <math.h>
#include "agent.h"
#include "utility.h"

/*Exception for when a path cannot be found in a grid*/
const char* Agent::NoPathException::what() const throw() {
	return "A path cannot be found!\n";
}

/*Constructor*/
Agent::Agent(Grid*& g, Position p, char d, Robot& r) : grid(g), pos(p), direction(d), robot(&r) {
    pos.setVisited(true);
}

/*Destructor*/
Agent::~Agent() {}

/*Sets direction to d if a valid direction*/
void Agent::setDirection(char d) {
    if( (d == 'n' || d == 'N') || (d == 's' || d == 'S') || (d == 'e' || d == 'E') || (d == 'w' || d == 'W') )
        direction = d;
}
/*Sets grid to g*/
void Agent::setGrid(Grid*& g) {grid = g;}
/*Sets pos to p*/
void Agent::setPosition(Position p) {pos = p;}

/*Returns pos*/
Position Agent::getPosition() {return pos;}
/*Returns grid*/
Grid*& Agent::getGrid() {return grid;}
/*Returns direction*/
char& Agent::getDirection() {return direction;}
/*Returns robot*/
Robot*& Agent::getRobot() {return robot;}


/*Returns true if the robot can step from one position to another*/
bool Agent::checkStepValidity(Position& a, Position& b) {

    //if they share a row
    if(a.getRow() == b.getRow()) {
        if(abs(a.getCol()-b.getCol()) < 2)
            return true;
        else
            return false;
    }

    else if(a.getCol() == b.getCol()) {
        if(abs(a.getRow()-b.getRow()) < 2)
            return true;
        else
            return false;
    }
}   //END CHECKSTEPVALIDITY


/*Returns if a position is in bounds, unvisited, and accessible*/
bool Agent::positionValid(Position& p) {
    if( (p.getRow() < grid->getNumOfRows()) && (p.getCol() < grid->getNumOfCols()) &&  (!p.getVisited()) && (grid->getPos(p.getRow(), p.getCol()) == ' ') )
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

/*Returns the Node that the overseer decides on*/
Tree::Node* Agent::confirmMove(Tree::Node*& n, Tree::Node*& p, PriorityQueue& pq) {

    //mainly for the first time entering the while loop in traverse
    if(n->getValue().equals(pos))
        return n;


    Tree::Node* result = p;
    bool done = false;

    while(!done) {

        std::string input;
        std::cout<<grid->toString();
        std::cout<<"\n1. Move to Position "<<n->getValue().toString();
        std::cout<<"\n2. Enter new Position";
        std::cout<<"\n3. Print priority queue";
        std::cout<<"\n4. Spin for sensors\n";
        std::cin>>input;
        std::cin.get();  //get any remaining chars

        int temp = atoi(input.c_str());

        if(temp == 3)
            std::cout<<"\n"<<pq.toString();

        else if(temp == 1) {
            result = n;
            done = true;
        }

        else if(temp == 4) {
            std::cout<<"\nHow many degrees?\nRange: -360 - 360 MUST BE IN INCREMENTS OF 90\n";
            std::cin>>input;
            int d = atoi(input.c_str());
            if(d % 90 == 0)
                robot->turnXDegrees(d);
            else
                std::cout<<"\nInvalid value\n";
        }


        else if(temp == 2) {
            bool valid = false;
            Position temp;
            std::cout<<"\n\nEnter the row for new Position\n";
            getline(std::cin, input);
            temp.setRow(atoi(input.c_str()));

            std::cout<<"\nEnter the column for new Position\n";
            getline(std::cin, input);
            temp.setCol(atoi(input.c_str()));

            //make sure position is valid
            if( (temp.getRow() >= grid->getNumOfRows()) || (temp.getRow() < 0) || (temp.getCol() >= grid->getNumOfCols()) || (temp.getCol() < 0)
               || (grid->getPos(temp.getRow(), temp.getCol()) == '#') )
                std::cout<<temp.toString()<<" is not a valid position!\n";
            else
                valid = true;

            if(valid) {
                done = true;
                Tree::Node* tempN = new Tree::Node(temp, p);

                //if inputted same value that was passed
                if(result->getValue().equals(n->getValue()))
                    result = n;

                result = tempN;
            }   //end if valid input
        }   //end else if 2

        else
            std::cout<<"\nInvalid Input\n";
    }   //end while

    //set grid
    grid->setPos(p->getValue().getRow(), p->getValue().getCol(), ' ');
    grid->setPos(result->getValue().getRow(), result->getValue().getCol(), 'x');
    return result;
}   //END CONFIRMMOVE


/*
  Returns a path from one position to another
  Throws exception if a path cannot be found
  if oversee is true, it calls confirmMove before each move
  if trace is true, the robot moves as the agent moves
*/
Path Agent::traverse(Position& start, Position& end, bool oversee, bool trace) {

    //make tree and add the start position as the root node
    Tree* tree = new Tree(start);

    //set root node values
    tree->getRoot()->setGValue(0);
    tree->getRoot()->setHValue(getSLDistance(start,end));
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
    grid->setPos(current->getValue().getRow(), current->getValue().getCol(), 'x');  //set initial spot on grid

    //start traversing
    while(!done) {

        //if the queue is empty, there is no path
        if(nodes.isEmpty())
            throw new NoPathException();

        //if we are tracing, set parent to current before changing current
        if(trace)
            parent = current;

        //if oversee is true, ask for an action
        if(oversee)
            //move to the decided position
            current = confirmMove(nodes.top(), current, nodes);

        //else, set grid and set current to best node
        else {
            grid->setPos(current->getValue().getRow(), current->getValue().getCol(), ' ');
            current = nodes.top();
            grid->setPos(current->getValue().getRow(), current->getValue().getCol(), 'x');
            std::cout<<grid->toString();
        }

        //if we are tracing, check the step validity
        if(trace) {
            //if true then just step
            if(checkStepValidity(parent->getValue(), current->getValue()))
                robot->step(parent->getValue(), current->getValue(), direction);
            //else, recurse to find a path and step through that
            else {
                grid->clear();
                grid->setPos(parent->getValue().getRow(), parent->getValue().getCol(), 'x');
                Path temp = traverse(parent->getValue(), current->getValue(), false, false);
                robot->stepPath(temp, direction);
            }   //end else
        }   //end if trace

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

    //check validity of path
    for(int i=result.getPath().size()-1;i>0;i--) {
        //if not a valid step, recurse to find path between two positions and concatenate that to the path
        if(!checkStepValidity(result.getPath().at(i), result.getPath().at(i-1))) {

            grid->clear();
            pos = result.getPath().at(i);

            std::cout<<"\nFILLING PATH BETWEEN "<<result.getPath().at(i).toString()<<" AND "<<result.getPath().at(i-1).toString();
            Path temp = traverse(result.getPath().at(i), result.getPath().at(i-1), false, false);
            temp.arrange();

            for(int j=temp.getPath().size()-2;j>0;j--)
                result.insert(temp.getPath().at(j),i);

        }   //end if
    }   //end for
    result.arrange();
    return result;
}   //END TRAVERSE

