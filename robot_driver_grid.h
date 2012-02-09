
#ifndef ROBOT_DRIVER_GRID_H
#define ROBOT_DRIVER_GRID_H
#include <string>
#include <string.h>
#include <stdio.h>
#include <exception>
#include "robot_driver_path.h"

class Grid {
public:


	const static char PATH = '.';

    //! Exception class for a file that does not exist
	class FileNotFoundException: public std::exception {
	public:
		virtual const char* what() const throw() {
			return "\nThe file could not be found!\n";
		};
	};  //end class EmptyTreeException


    Grid();

    //! A constructor
    /*!
     * Constructor that takes in a filename for a grid\n
     * Determines and sets map, numRows, numCols, and accessible
     */
	explicit Grid(std::string&);
    //! A constructor
    /*! Constructor that sets map to themap, numRows to r, and numCols to c */
	Grid(char**, int&, int&);

    //! Destructor
    /*! Deletes map */
	~Grid();

    //! Getter function for themap member
    /*! Returns the value of map member */
	char** getMap();

	void setMap(char**, int&, int&);

    //! Getter function for numRows member
    /*! Returns numRows member value */
	int getNumOfRows();

    //! Getter function for numCols member
    /*! Returns numCols member value */
	int getNumOfCols();


	//! Getter function for f_name member
	/*! Returns the value of f_name */
	const std::string& getFilename();;

    //! Returns value at specific index
    /*! Returns the value of map at r,c */
	char getPos(int&, int&);

    //! Set an index of map to specified char
    /*! Set map[row][col] to value */
	void setPos(int&, int&, char);


    //! Clears grid
    /*! Sets all accessible positions to ' ' */
    void clear();

    //! Marks specified path on grid
    /*!
     * Clears grid and marks only positions in p on grid\n
     * First position is marked as 'x' and rest as '.'
     */
    void markPath(Path&, char);

    //! Returns a printable string of the grid
	const std::string toString();

private:
    const std::string f_name;
	char** map;
	int num_rows;
	int num_cols;
};

#endif
