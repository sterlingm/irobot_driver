#include <sstream>
#include "path.h"

/*Blank constructor*/
Path::Path() {}
/*Constructor that sets path to p*/
Path::Path(std::vector<Position>& p) : path(p) {}

/*Destructor*/
Path::~Path() {}

/*Returns path*/
std::vector<Position>& Path::getPath() {return path;};
/*Returns the size of the path vector*/
int Path::getSize() {return path.size();}

/*Pushes back p onto the path*/
void Path::add(Position& p) {path.push_back(p);}

/*Inserts p at the given index*/
void Path::insert(Position& p, int index) {
    Position temp = path.at(path.size()-1);

    for(int i=path.size()-1;i>index;i--)
        path.at(i) = path.at(i-1);

    path.at(index) = p;
    add(temp);
}   //END INSERT

/*Rearranges vector to not be in reverse order*/
void Path::arrange() {
    for(int i=0;i<path.size()/2;i++) {

        Position temp = path.at(path.size()-1-i);
        path.at(path.size()-1-i) = path.at(i);
        path.at(i) = temp;

    }
}   //END ARRANGE


bool Path::contains(Position& p) {

    for(int i=0;i<path.size()/2;i++)
        if( (path.at(i).equals(p)) || (path.at(path.size()-1-i).equals(p)) )
            return true;

    return false;

}   //END CONTAINS


/*Returns a string of the path*/
std::string Path::toString() {
    std::ostringstream result;

    for(int i=1;i<path.size();i++)
        result<<" "<<path.at(i).toString();

    return result.str();
}   //END TOSTRING

