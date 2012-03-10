

#ifndef ROBOT_DRIVER_POSITION_H
#define ROBOT_DRIVER_POSITION_H

#include <string>

class Position {

public:

    //! A Constructor
    /*! Sets row and col to -1 */
	Position();

    //! A Constructor
    /*! Sets row to r and col to c */
	Position(int, int);

    //! A Destructor
	~Position();

    //! Getter function for row member
    /*! Returns a reference of row member */
	int& getRow();

	//! Setter function for row
	/*! Sets row to r */
	void setRow(int&);

	//! Getter function for col
	/*! Returns a reference of col member */
	int& getCol();

	//! Setter function for col
	/*! Sets col to c */
	void setCol(int&);

	//! Checks if this position is equal to specified Position
	/*! Returns true if row and col are equal in both Positions */
	bool equals(Position&);

	//! Function to clone Position
	/*! Returns a Position instance that has row and col equal to this Position's members */
    Position clone();

	//! Returns a printable string of the Position
	const std::string toString();
private:
	int row;
	int col;
};

#endif
