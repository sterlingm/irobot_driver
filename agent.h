#ifndef AGENT_H
#define AGENT_H

#include <vector>
#include "grid.h"
#include "path.h"
#include "Robot.h"
#include "tree.h"
#include "priorityqueue.h"

class Agent {

public:

	class NoPathException : public std::exception {
	public:
		virtual const char* what() const throw();
	};	//end exception

    Agent(Grid*&, Position, char, Robot&);
    ~Agent();

    //getters, setters
    char& getDirection();
    void setDirection(char);
    Position getPosition();
    void setPosition(Position);
    bool posVisited(Position&);
    Grid*& getGrid();
    void setGrid(Grid*&);
    Robot*& getRobot();

    bool positionValid(Position&);

    double getSLDistance(Position&,Position&);

    bool processCommand(int);

    bool checkStepValidity(Position&, Position&);

    std::vector<Position> adjacentPositions(Position&);

    Tree::Node* confirmMove(Tree::Node*&, Tree::Node*&, PriorityQueue&);    //ask overseer for direction

    Path traverse(Position&, Position&, bool, bool);


private:
    Robot* robot;
    Grid* grid;
    char direction;
    Position pos;
};
#endif
