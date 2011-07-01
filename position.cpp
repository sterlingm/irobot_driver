#include "position.h"
#include <sstream>

/*Constructor that sets values to -1 and false*/
Position::Position() : row(-1), col(-1), visited(false) {}

/*Constructor*/
Position::Position(int r, int c) : row(r), col(c), visited(false) {}

/*Destructor*/
Position::~Position() {}

/*Returns row*/
int& Position::getRow() {return row;}
/*Returns col*/
int& Position::getCol() {return col;}
/*Returns visited*/
bool Position::getVisited() {return visited;}

/*Sets row to r*/
void Position::setRow(int r) {row = r;}
/*Sets col to c*/
void Position::setCol(int c) {col = c;}
/*Sets visited to v*/
void Position::setVisited(bool v) {visited = v;}

/*Returns true if all members equal the members in b*/
bool Position::equals(Position& b) {
    if( (row == b.getRow()) && (col == b.getCol()) )
        return true;
    return false;
}   //END EQUALS

/*Returns a Position with all the same values*/
Position Position::clone() {
    Position result;
    result.row = row;
    result.col = col;
    result.visited = visited;
    return result;
}   //END CLONE

/*Returns a string in the form of (row,col)*/
std::string Position::toString() {
	std::ostringstream result;
	result<<"("<<row<<","<<col<<")";
	return result.str();
}   //END TOSTRING
