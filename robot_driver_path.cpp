#include <sstream>
#include "robot_driver_path.h"

/*Blank constructor*/
Path::Path() : size(0), capacity(20) {path.reserve(20);}
/*Constructor that sets path's size to parameter*/
Path::Path(int& s) : size(0), capacity(s) {path.reserve(s);}
/*Constructor that sets path to p*/
Path::Path(std::vector<Position>& p) : path(p), size(p.size()), capacity(p.capacity()) {}

/*Destructor*/
Path::~Path() {}

/*Returns path*/
std::vector<Position>& Path::getPath() {return path;};
/*Returns the size of the path vector*/
int Path::getSize() {return size;}

/*Pushes back p onto the path*/
void Path::add(Position& p) {

    //check if we need to reserve more
    if(++size == capacity) {
        capacity *= 1.5;
        path.reserve(capacity);
    }   //end if
    //push back
    path.push_back(p);
}   //END ADD

/*Inserts p at the given index*/
void Path::insert(Position& p, int index) {

    //hold last position
    Position temp = path[size-1];
    //rearrange queue
    for(int i=size-1;i>index;i--)
        path[i] = path[i-1];
    //insert
    path[index] = p;
    //add temp back
    add(temp);
}   //END INSERT

/*Removes the front Position of the Path*/
void Path::pop_front() {
    path.erase(path.begin());
    size--;
}   //END POP_FRONT

/*Removes the last Position of the Path*/
void Path::pop_back() {
    path.erase(path.end());
    size--;
}   //END POP_BACK

/*Clears the path of all positions*/
void Path::clear() {
    while(path.size() > 0)
        path.pop_back();
    size = 0;
}   //END CLEAR

/*Rearranges vector to not be in reverse order*/
void Path::reverse() {
    for(int i=0;i<size/2;i++) {
        Position temp = path[size-1-i];
        path[size-1-i] = path[i];
        path[i] = temp;
    }   //end for
}   //END ARRANGE


bool Path::contains(Position& p) {

    for(int i=0;i<size/2;i++)
        if( (path[i].equals(p)) || (path[size-1-i].equals(p)) )
            return true;

    return false;

}   //END CONTAINS


/*Returns a string of the path*/
const std::string Path::toString() {
    std::ostringstream result;

    for(int i=1;i<size;i++)
        result<<" "<<path[i].toString();

    return result.str();
}   //END TOSTRING

