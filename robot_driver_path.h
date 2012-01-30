#ifndef ROBOT_DRIVER_PATH_H
#define ROBOT_DRIVER_PATH_H

#include <vector>
#include "robot_driver_position.h"

class Path {
public:
    //! Constructor
    /*! Defaults the vector to reserve 20 elements */
    Path();

    //! Constructor
    /*! Reserves s elements in the vector */
    explicit Path(int&);

    //! A Constructor
    /*! Sets path member to p */
    explicit Path(std::vector<Position>&);
    //! A destructor
    ~Path();

    //! Getter function for path member
    /*! Returns a reference to path member */
    std::vector<Position>& getPath();

    //! Getter for size member
    /*! Returns value of size member */
    int getSize();

    //! Adds a position to back of the path
    /*! Adds p to the back of the path vector */
    void add(Position&);

    //! Inserts a position at specific index
    /*! Inserts p at index of the path */
    void insert(Position&, int);

    //! Removes the front Position of the Path
    void pop_front();

    //! Removes the last Position of the Path
    void pop_back();

    //! Empties the path
    /*! Removes all elements from the vector of Positions */
    void clear();

    //! Reverses order of path vector contents
    /*! Reverses order of path vector contents */
    void reverse();

    //! Checks if p is in the path
    /*! Returns true if path vector contains p */
    bool contains(Position&);

    //! Returns a printable string of the path
    const std::string toString();

private:
    std::vector<Position> path;
    int size;
    int capacity;
};
#endif
