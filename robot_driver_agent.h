#ifndef ROBOT_DRIVER_AGENT_H
#define ROBOT_DRIVER_AGENT_H

#include <vector>
#include "robot.h"
#include "robot_driver_grid.h"
#include "robot_driver_path.h"
#include "robot_driver_tree.h"
#include "robot_driver_priorityqueue.h"

class Agent {

public:

    //! Exception for when a path does not exist
	class NoPathException : public std::exception {
	public:
		virtual const char* what() const throw();
	};	//end exception

    Agent();

    Agent(Robot&);

    Agent(Robot&, int);

    //! A constructor
    /*! Initializes grid, robot, and direction to parameters */
    Agent(Grid*&, Robot&, int);

    //! Destructor
    ~Agent();


    //! Getter function for direction
    /*! Returns reference to direction member */
    int& getDirection();
    //! Setter function for direction
    /*!
     * Sets direction member to d\n
     */
    void setDirection(int);


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
    /*! Sets grid member to g*/
    void setGrid(Grid*);


    //! Getter function for robot
    /*! Returns a reference to robot member */
    Robot*& getRobot();


    //! Getter function for sensor value high byte
    /*! Returns the value of the sensor value high byte */
    int& getHighSV();
    //! Setter function for sensor value high byte
    /*! Sets highsv to v */
    void setHighSV(int&);


    //! Getter function for sensor value low byte
    /*! Returns the value of the sensor value low byte */
    int& getLowSV();
    //! Setter function for sensor value low byte
    /*! Sets lowsv to v */
    void setLowSV(int&);

    //! Getter function for spinning member\nReturns true if robot is spinning
    /*! Returns true if the robot is spinning */
    bool is_spinning();
    //! Setter function for spinning member
    void set_spinning(bool);

    //! Getter function for driving member\nReturns true if robot is driving
    bool is_driving();
    //! Setter function for driving member
    void set_driving(bool);

    //! Getter for currentSensor member
    /*! Returns a reference to currentSensor member */
    int& getCurrentSensor();
    //! Setter for currentSensor member
    /*! Sets currentSensor to s */
    void setCurrentSensor(int&);

    //! Getter for mode member
    /*! Returns f for full mode, s for safe mode, or p for passive mode*/
    char& get_mode();

    //! Setter for mode member
    /*! Sets mode to m*/
    void set_mode(char&);

    int get_algorithm();

    void set_algorithm(int&);

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
    Path astar_path(Position&,Position&);

    Path rrt_path(Position&, Position&);


    //! Moves the robot through path
    /*!
     * The robot moves through the path member\n
     * If it cannot move to the end, the robot moves as far as it can
     */
    void stepPath(bool);

    //! Changes the robot's direction
    /*!
     *  Changes the robot's direction
     *  Returns true on success, false on failure
     */
    void change_direction(int);


    //! Moves the robot from one position to another
    /*!
     * Moves the robot from Position a to Position b, if possible\n
     * First parameter is Position a\n
     * Second Parameter is Position b\n
     * If move is successful, the new Position is set\n
     */
    void step(Position&, Position&);

private:
    int direction;
    int currentSensor;
    int highsv;
    int lowsv;
    int algorithm;
    bool spinning;
    bool driving;
    char mode;
    Robot* robot;
    Grid* grid;
    Position pos;
    Position goal;
    Path path;

    Tree::Node* find_closest_node_in_tree(Tree*&, Position&);
    Position find_next_best(Tree&, Position);
    std::vector<Position> get_potential_samples(int&, int&);
};
#endif
