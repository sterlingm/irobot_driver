
/*
 *  Grid.cpp
 *  3153Project
 */

#include "grid.h"
#include <sstream>
#include <fstream>
#include <iostream>

/*
  Constructor
  Reads in a file, creates a 2D char array from file contents
  Throws FileNotFoundException if the file does not exist
*/
Grid::Grid(std::string& filename) {
    std::stringstream tempmap;	//temporary hold map
	char receive[256];	//get lines from file
	numOfRows = 0;
	numOfCols = 0;
	std::ifstream thefile(filename.c_str(), std::ios::in);


	//make sure the file exists
	if(!thefile)
		throw FileNotFoundException();


	//go through file and get map
	while(thefile.getline(receive, 256)) {
		numOfRows++;	//add a row
		//std::cout<<receive<<std::endl;
		tempmap<<receive<<std::endl;	//pass line into tempmap
		//set column dimension
		numOfCols = strlen(receive);
	}	//end while


	//make 2d char array
	map = new char*[numOfRows];
	for(int i=0;i<numOfRows;i++)
		map[i] = new char[numOfCols];


	//initialize char array
	for(int r=0;r<numOfRows;r++)
		for(int c=0;c<numOfCols;c++) {
			//if newline char, consume that
			if((char)tempmap.peek() == '\n')
				tempmap.get();
			map[r][c] = (char)tempmap.get();

            if(map[r][c] == ' ') {
                Position temp(r,c);
                accessible.push_back(temp);
            }
		}   //end inner for
}	//END CONSTRUCTOR

/*Constructor
  Takes in a 2D array and row/col values*/
Grid::Grid(char**& themap, int& r, int& c) : map(themap), numOfRows(r), numOfCols(c) {}


/*Destructor that deletes the 2D array*/
Grid::~Grid() {
	for(int r=0;r<numOfRows;r++)
		delete [] map[r];
}   //END DESTRUCTOR


/*Returns map*/
char**& Grid::getMap() {return map;}
/*Returns numOfRows*/
int Grid::getNumOfRows() {return numOfRows;}
/*Returns numOfCols*/
int Grid::getNumOfCols() {return numOfCols;}

/*Returns the value of the 2D array at row,col*/
char Grid::getPos(int row, int col) {return map[row][col];}

/*Sets the value of the 2D array at row,col to value*/
void Grid::setPos(int row, int col, char value) {map[row][col] = value;}

/*Returns accessible*/
std::vector<Position> Grid::getAccessiblePos() {return accessible;}

/*Clears all the accessible positions in the grid*/
void Grid::clear() {
    for(int i=0;i<accessible.size();i++)
        map[accessible.at(i).getRow()][accessible.at(i).getCol()] = ' ';
}   //END CLEAR

/*Clears the grid and then marks path p on the grid*/
void Grid::markPath(Path& p) {

    clear();
    setPos(p.getPath().at(p.getSize()-1).getRow(), p.getPath().at(p.getSize()-1).getCol(), 'x');
    for(int i=0;i<p.getSize()-1;i++)
        setPos(p.getPath().at(i).getRow(), p.getPath().at(i).getCol(), '.');

}   //END MARKPATH

/*Returns a string of the grid*/
std::string Grid::toString() {
	std::ostringstream result;
	result<<"\n";
	for(int row=0;row<numOfRows;row++) {
		for(int col=0;col<numOfCols;col++) {
			result<<getPos(row, col);
			if(col == numOfCols-1)	//if at the end of line, print \n
				result<<"\n";
		}	//end inner for
	}	//end outter for

	return result.str();
}	//END TOSTRING
