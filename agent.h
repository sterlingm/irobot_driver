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

    //! Exception for when a path does not exist
	class NoPathException : public std::exception {
	public:
		virtual const char* what() const throw();
	};	//end exception


    //! A constructor
    /*! Initializes grid, robot, and direction to parameters */
    Agent(Grid*&, Robot&, char);

    //! Destructor
    ~Agent();


    //! Getter function for direction
    /*! Returns reference to direction member */
    char& getDirection();
    //! Setter function for direction
    /*!
     * Sets direction member to d\n
     * if d is e(east), E(east), w(west), W(west), s(south), S(south), n(north), or N(north)
     */
    void setDirection(char);


    //! Getter function for pos
    /*! Returns a reference to pos member */
    Position& getPosition();
    //! Setter function for pos
    /*! Sets pos member to p */
    void setPosition(Position&);


    //! Getter function for goal
    /*! Returns a reference to goal member */
    Position& getGoal();
    //! Setter function for goal
    /*! Sets goal to g */
    void setGoal(Position&);


    //! Getter function for path
    /*! Returns a reference to path member */
    Path& getPath();
    //! Setter function for path
    /*! Sets path to p */
    void setPath(Path&);



    //! Getter function for grid
    /*! Returns a reference to grid member */
    Grid*& getGrid();
    //! Setter function for grid
    /*! Sets grid member to g */
    void setGrid(Grid*&);



    //! Getter function for robot
    /*! Returns a reference to robot member */
    Robot*& getRobot();
    //! Setter function for robot
    /*! Sets robot member to r */
    void setRobot(Robot&);


    //! Getter function for sensor value high byte
    /*! Returns the value of the sensor value high byte */
    int getHighSV();
    //! Setter function for sensor value high byte
    /*! Sets highsv to v */
    void setHighSV(int&);


    //! Getter function for sensor value low byte
    /*! Returns the value of the sensor value low byte */
    int getLowSV();
    //! Setter function for sensor value low byte
    /*! Sets lowsv to v */
    void setLowSV(int&);


    //! Returns true if position has been visited
    /*!
     * Returns true if Position reference passed is\n
     * within grid member bounds, not visited, and equal to ' '
     */
    bool positionValid(Position&);


    //! Returns straight line distance between two Positions
    /*! Returns straight line distance between two Position references passed */
    double getSLDistance(Position&,Position&);

    //! Returns vector of adjacent positions to the parameter position
    /*! Returns a vector of Positions that are adjacent to the Position reference passed */
    std::vector<Position> adjacentPositions(Position&);

    //! Returns a path from pos to the parameter
    /*!
     * Returns a Path from pos member to the Position reference passed\n
     * Throws a NoPathException if no path exists
     */
    Path traverse(Position&);


    //! Moves the robot through path
    /*!
     * The robot moves through the path member\n
     * If it cannot move to the end, the robot moves as far as it can
     */
    void stepPath();

private:
    Robot* robot;
    Grid* grid;
    char direction;
    Position pos;   //current position
    Position goal;  //current goal
    Path path;
    int highsv;
    int lowsv;

};
#endif
