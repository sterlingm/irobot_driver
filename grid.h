
#ifndef GRID_H
#define GRID_H
#include <string>
#include <string.h>
#include <stdio.h>
#include <exception>
#include "path.h"

class Grid {
public:


	class FileNotFoundException: public std::exception {
	public:
		virtual const char* what() const throw() {
			return "\nThe file could not be found!\n";
		};
	};  //end class EmptyTreeException


	const static char PATH = '.';
	const static char LOC = 'X';


	Grid(std::string&);
	Grid(char**&, int&, int&);
	~Grid();

	char**& getMap();
	int getNumOfRows();
	int getNumOfCols();
	std::vector<Position> getAccessiblePos();

	char getPos(int, int);
	void setPos(int, int, char);

    void clear();
    void markPath(Path&);

	std::string toString();

private:
	char** map;
	int numOfRows;
	int numOfCols;
	std::vector<Position> accessible;
};

#endif
