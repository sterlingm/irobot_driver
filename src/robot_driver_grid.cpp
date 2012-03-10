

#include "robot_driver_grid.h"
#include <sstream>
#include <fstream>
#include <iostream>


Grid::Grid() : map(0) {}

Grid::Grid(char** m, int& r, int& c) {setMap(m,r,c);}
/*
  Constructor
  Reads in a file, creates a 2D char array from file contents
  Throws FileNotFoundException if the file does not exist
*/
Grid::Grid(std::string& filename) : f_name(filename) {
    std::stringstream tempmap;	//temporary hold map
	char receive[256];	//get lines from file
	num_rows = 0;
	num_cols = 0;
	std::ifstream thefile(filename.c_str(), std::ios::in);


	//make sure the file exists
	if(!thefile)
		throw FileNotFoundException();


	//go through file and get map
	while(thefile.getline(receive, 256)) {
		num_rows++;	//add a row
		//std::cout<<receive<<std::endl;
		tempmap<<receive<<std::endl;	//pass line into tempmap
		//set column dimension
		num_cols = strlen(receive);
	}	//end while


	//make 2d char array
	map = new char*[num_rows];
	for(int i=0;i<num_rows;i++)
		map[i] = new char[num_cols];


	//initialize char array
	for(int r=0;r<num_rows;r++)
		for(int c=0;c<num_cols;c++) {
			//if newline char, consume that
			if((char)tempmap.peek() == '\n')
				tempmap.get();
			map[r][c] = (char)tempmap.get();
		}   //end inner for
}	//END CONSTRUCTOR


/*Destructor that deletes the 2D array*/
Grid::~Grid() {
	for(int r=0;r<num_rows;r++) {
		delete [] map[r];
        map[r] = 0;
	}
    delete [] map;
    map = 0;
}   //END DESTRUCTOR


/*Returns map*/
char** Grid::getMap() {return map;}
/*Sets the map to m*/
void Grid::setMap(char** map, int& max_r, int& max_c) {

    /*std::cout<<"\nMap passed to setMap:\n";
    std::cout<<"\n"<<map;
    for(int r=0;r<max_r;r++)
        std::cout<<"\nr:"<<r<<" map["<<r<<"]:"<<map[r];
    std::cout<<"\n";*/

    for(int r=0;r<max_r;r++)
        for(int c=0;c<max_c;c++) {
            std::cout<<map[r][c];
            if(c==max_c-1) std::cout<<"\n";
        }

    //std::cout<<"\ndeleting:"<<this->map;
    //delete map
    if(this->map != 0) {
        for(int r=0;r<num_rows;r++) {
            delete [] this->map[r];
            this->map[r] = 0;
        }
        delete [] this->map;
        this->map = 0;
    }

    //remake 2d char array with new dimensions
	this->map = new char*[max_r];
	for(int r=0;r<max_r;r++)
		this->map[r] = new char[max_c];



    //set map
    for(int r=0;r<max_r;r++)
        for(int c=0;c<max_c;c++)
            this->map[r][c] = map[r][c];

    //set dimensions
    num_rows = max_r;
    num_cols = max_c;
}   //END SETMAP

/*Returns num_rows*/
int Grid::getNumOfRows() {return num_rows;}

/*Returns num_cols*/
int Grid::getNumOfCols() {return num_cols;}

/*Returns the value of the 2D array at row,col*/
char Grid::getPos(int& row, int& col) {return map[row][col];}

/*Sets the value of the 2D array at row,col to value*/
void Grid::setPos(int& row, int& col, char value) {map[row][col] = value;}

/*Returns f_name*/
const std::string& Grid::getFilename() {return f_name;}

/*Clears all the accessible positions in the grid*/
void Grid::clear() {
    for(int r=0;r<num_rows;r++)
        for(int c=0;c<num_cols;c++)
            if(map[r][c] != '#')
                map[r][c] = ' ';
    //for(int i=0;i<accessible.size();i++)
        //map[accessible.at(i).getRow()][accessible.at(i).getCol()] = ' ';
}   //END CLEAR

/*Clears the grid and then marks path p on the grid*/
void Grid::markPath(Path& p, char id) {
    //std::cout<<"\nmarking path "<<p.toString()<<"\n";
    clear();
    setPos(p.getPathVector().at(0).getRow(), p.getPathVector().at(0).getCol(), id);

    for(int i=1;i<p.getSize();i++)
        //check for duplicate at beginning
        if( (i == 1) && (p.getPathVector().at(0).equals(p.getPathVector().at(1))) ) {}

        else
            setPos(p.getPathVector().at(i).getRow(), p.getPathVector().at(i).getCol(), PATH);

}   //END MARKPATH



/*Returns a string of the grid*/
const std::string Grid::toString() {
	std::ostringstream result;
	for(int row=0;row<num_rows;row++) {
		for(int col=0;col<num_cols;col++) {
			result<<getPos(row, col);
			if(col == num_cols-1)	//if at the end of line, print \n
				result<<"\n";
		}	//end inner for
	}	//end outter for

	return result.str();
}	//END TOSTRING
