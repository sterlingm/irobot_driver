
#ifndef GRID_H
#define GRID_H
#include <string>
#include <string.h>
#include <stdio.h>
#include <exception>
#include "path.h"

class Grid {
public:


	const static char PATH = '.';
	const static char LOC = 'X';

    //! Exception class for a file that does not exist
	class FileNotFoundException: public std::exception {
	public:
		virtual const char* what() const throw() {
			return "\nThe file could not be found!\n";
		};
	};  //end class EmptyTreeException


    //! A constructor
    /*!
     * Constructor that takes in a filename for a grid
     * Determines and sets map, numRows, numCols, and accessible
     */
	Grid(std::string&);
    //! A constructor
    /*!
     * Constructor that sets map to themap, numRows to r, and numCols to c
     */
	Grid(char**&, int&, int&);

    //! Destructor
    /*!
     * Deletes map
     */
	~Grid();

    //! Getter function for themap member
    /*!
     * Returns a char**& reference of map member
     */
	char**& getMap();

    //! Getter function for numRows member
    /*!
     * Returns numRows member value
     */
	int getNumOfRows();

    //! Getter function for numCols member
    /*!
     * Returns numCols member value
     */
	int getNumOfCols();

    //! Getter function for accessible member
    /*!
     * Returns accessible member value
     */
	std::vector<Position> getAccessiblePos();

    //! Returns value at specific index
    /*!
     * Returns the value of map at r,c
     */
	char getPos(int, int);

    //! Set an index of map to specified char
    /*!
     * Set map[row][col] to value
     */
	void setPos(int, int, char);

    //! Clears grid
    /*!
     * Sets all accessible positions to ' '
     */
    void clear();

    //! Marks specified path on grid
    /*!
     * Clears grid and marks only positions in p on grid
     * First position is marked as 'x' and rest as '.'
     */
    void markPath(Path&);

    //! Returns a printable string of the grid
	std::string toString();

private:
	char** map;
	int numOfRows;
	int numOfCols;
	std::vector<Position> accessible;
};

#endif
