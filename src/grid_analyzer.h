#ifndef GRID_ANALYZER_H
#define GRID_ANALYZER_H
#include "robot_driver_grid.h"
#include "robot_driver_path.h"
#include "robot_driver_tree.h"
#include "robot_driver_priorityqueue.h"
#include "robot_driver_stack.h"
#include "robot_driver_utility.h"
#include <math.h>

class Grid_Analyzer {
public:

    //! Blank constructor
    Grid_Analyzer();
    //! Constructor
    /*! A constructor that sets the grid member to g*/
    Grid_Analyzer(Grid*);
    //! Destructor
    ~Grid_Analyzer();

    //! Getter function for grid member
    /*! Returns a reference to the grid member pointer*/
    Grid*& getGrid();
    //! Setter function for grid member
    /*! Sets grid member to g*/
    void setGrid(Grid*);


    //! Returns straight line distance between two Positions
    /*! Returns straight line distance between two Position references passed */
    double getEuclideanDistance(Position&,Position&);

    //! Returns vector of adjacent positions to the parameter position
    /*! Returns a vector of Positions that are adjacent to the Position reference passed */
    std::vector<Position> adjacentPositions(Position&);

    //! Returns true if position has been visited
    /*!
     * Returns true if Position reference passed is\n
     * within grid member bounds, not visited, and equal to ' '
     */
    bool positionValid(Position&);

    //! Returns an A* Path
    /*!
     * Returns an A* Path from init to goal based on grid member\n
     * If a Path cannot be found from init to goal,\n
     * it will return a path to the next-closest position to goal
     */
    Path astar_path(Position&, Position&);

    Path astar_path_decomposed(Position&, Position&, Tree*);

    Tree* build_rrt(Position&, Position&);

    //! Returns an RRT Path
    /*! Returns an RRT Path from init to goal based on grid member*/
    Path rrt_path(Position&, Position&);

private:
    Grid* grid;

    Position find_next_best(Tree&, Position);
    Tree::Node* find_nearest_neighbor(Tree*&, Position&);
    Path connect_for_rrt(Position&, Position&);
    std::vector<Position> get_cstate_subset(Position&, Position&, int&);
};
#endif
