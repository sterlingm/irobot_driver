#ifndef PATH_H
#define PATH_H

#include <vector>
#include "position.h"

class Path {
public:

    Path();
    ~Path();

    std::vector<Position>& getPath();
    int getSize();

    void add(Position&);
    void insert(Position&, int);

    void arrange();

    std::string toString();

private:
    std::vector<Position> path;
    int size;
};
#endif
