

#ifndef POSITION_H
#define POSITION_H

#include <string>

class Position {
	friend std::ostream& operator<<(std::ostream&, Position*&);
public:

	Position();
	Position(int, int);
	~Position();

	bool getVisited();
	int& getRow();
	int& getCol();
	void setRow(int);
	void setCol(int);
	void setVisited(bool);

	bool equals(Position&);
    Position clone();

	std::string toString();
private:
	int row;
	int col;
	bool visited;
};

#endif
