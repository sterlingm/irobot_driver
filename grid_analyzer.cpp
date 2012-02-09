#include "grid_analyzer.h"
#include <time.h>
#include <stdlib.h>
#include <iostream>

Grid_Analyzer::Grid_Analyzer() : grid(0) {}
Grid_Analyzer::Grid_Analyzer(Grid* g) : grid(g) {}
Grid_Analyzer::~Grid_Analyzer() {}


Grid*& Grid_Analyzer::getGrid() {return grid;}
void Grid_Analyzer::setGrid(Grid* g) {grid = g;}

/*Returns the straight line distance between two positions*/
double Grid_Analyzer::getSLDistance(Position& a, Position& b) {
    return sqrt( pow(a.getRow()-b.getRow(), 2) + pow(a.getCol()-b.getCol(),2) );
}   //END GETSLDISTANCE


/*Returns if a position is in bounds, unvisited, and accessible*/
bool Grid_Analyzer::positionValid(Position& p) {
    if( (p.getRow() < grid->getNumOfRows()) && (p.getCol() < grid->getNumOfCols()) &&  ((grid->getPos(p.getRow(), p.getCol()) == ' ')) )
        return true;
    return false;
}   //END POSITIONVALID



/*Returns a vector of the valid positions adjacent to given position*/
std::vector<Position> Grid_Analyzer::adjacentPositions(Position& p) {
    std::vector<Position> result;
    int startr, endr, startc, endc;

    if( (p.getRow() == 0) && (p.getCol() == 0) ) {
        startr = 0;
        startc = 0;
        endr = 1;
        endc = 1;
    }
    else if( (p.getRow() == grid->getNumOfRows()) && (p.getCol() == grid->getNumOfCols()) ) {
        startr = grid->getNumOfRows()-1;
        endr = grid->getNumOfRows();
        startc = grid->getNumOfCols()-1;
        endc = grid->getNumOfCols();
    }
    else if(p.getRow() == 0) {
        startr = 0;
        endr = 1;
        startc = p.getCol()-1;
        endc = p.getCol()+1;
    }
    else if(p.getCol() == 0) {
        startr = p.getRow()-1;
        endr = p.getRow()+1;
        startc = 0;
        endc = 1;
    }
    else if(p.getRow() == grid->getNumOfRows()) {
        startr = grid->getNumOfRows()-1;
        endr = grid->getNumOfRows();
        startc = p.getCol()-1;
        endc = p.getCol()+1;
    }
    else if(p.getCol() == grid->getNumOfCols()) {
        startr = p.getRow()-1;
        endr = p.getRow()+1;
        startc = grid->getNumOfCols()-1;
        endc = grid->getNumOfCols();
    }
    else {
        startr = p.getRow()-1;
        endr = p.getRow()+1;
        startc = p.getCol()-1;
        endc = p.getCol()+1;
    }


    for(int r=startr;r<=endr;r++)
        for(int c=startc;c<=endc;c++)
            //xor for 4-connectivity and not checking p
            if( !((p.getRow() == r) && (p.getCol() == c)) ) {
                Position temp(r,c);
                if(positionValid(temp))
                    result.push_back(temp);
            }   //end if


    return result;
}   //END GETADJACENTPOSITIONS


/*Finds the closest position in tree to position e*/
Position Grid_Analyzer::find_next_best(Tree& tree, Position e) {

    //std::cout<<"tree in find_next_best for "<<e.toString()<<":"<<tree.toString()<<"\n";

    Position result;
    Tree::Node* nb = tree.getNodes().at(0);
    for(int i=1;i<tree.getNodes().size();i++) {

        //std::cout<<"\ncomparing "<<nb->getValue().toString()<<" with "<<tree.getNodes().at(i)->getValue().toString();
        //std::cout<<"\nnb->getHValue: "<<nb->getHValue()<<" "<<tree.getNodes().at(i)->getValue().toString()<<"->HValue: "<<tree.getNodes().at(i)->getHValue();

        //check straight line distance
        if( tree.getNodes().at(i)->getHValue() < nb->getHValue() )
            nb = tree.getNodes().at(i);

        //if straight line distance is equal, check step cost
        else if( tree.getNodes().at(i)->getHValue() == nb->getHValue() )
            if( tree.getNodes().at(i)->getGValue() < nb->getGValue() )
                nb = tree.getNodes().at(i);

    }   //end for

    result = nb->getValue();

    //std::cout<<"\nFIND NEXT BEST RETURNING: "<<result.toString();
    return result;
}   //END FIND_NEXT_BEST





Path Grid_Analyzer::astar_path(Position& init, Position& goal) {
    //std::cout<<"\nA* path from "<<pos.toString()<<" to "<<end.toString()<<"\n";

    //make tree and add the start position as the root node
    Tree* tree = new Tree(init);

    //set root node values
    tree->getRoot()->setGValue(0);
    tree->getRoot()->setHValue(getSLDistance(init, goal));
    tree->getRoot()->setFValue();
    tree->getRoot()->setParent(tree->getRoot());

    //make priority queue of todo nodes and add the root to queue
    PriorityQueue nodes;
    nodes.push(tree->getRoot());

    //make path to return
    Path result;

    //flag to stop
    bool done = false;

    //the parent to current for robot tracing
    Tree::Node* parent = tree->getRoot();

    //best node
    Tree::Node* current = tree->getRoot();
    //**grid->setPos(current->getValue().getRow(), current->getValue().getCol(), 'x');  //set initial spot on grid

    //start traversing
    while(!done) {

        //if the queue is empty, there is no path
        if(nodes.isEmpty()) {
            //call find_next_best
            Position temp = find_next_best(*tree, goal);

            //set end to the next best position
            goal = temp;

            //set current to the next_best node to end the traversal
            current = tree->find(temp);

            //repush current onto queue so we don't come into this block again
            nodes.push(current);

        }   //end if queue empty

        //else there are more positions to try
        else {

            //std::cout<<"\nTREE WHEN CURRENT IS "<<current->getValue().toString()<<tree->toString()<<"\n";
            //set the new parent
            parent = current;


            //else, set grid and set current to best node
            //**grid->setPos(current->getValue().getRow(), current->getValue().getCol(), ' ');
            current = nodes.top();
            //**grid->setPos(current->getValue().getRow(), current->getValue().getCol(), 'x');
            //**std::cout<<grid->toString();

            //if agreed to best move, pop off node
            if(current == nodes.top())
                nodes.pop();

            //if we are at the goal, end loop
            if(current->getValue().equals(goal))
                done = true;


            //else, traverse further
            else {

                //get successor nodes for current
                std::vector<Position> successors = adjacentPositions(current->getValue());
                //std::cout<<"\nPositions adjacent to "<<current->getValue().toString();
                //for(int i=0;i<successors.size();i++)
                    //std::cout<<"\n"<<successors.at(i).toString();

                //for each successor
                for(int i=0;i<successors.size();i++) {

                    //if not equal to an ancestor node
                    if(!tree->contains(successors.at(i))) {

                        //make temp node for the tree
                        Tree::Node temp(successors.at(i), current);
                        //set the g value to current.gvalue + step cost (step cost is always 1)
                        temp.setGValue(current->getGValue() + 1);
                        //set h value
                        temp.setHValue(getSLDistance(temp.getValue(), goal));
                        //set f value
                        temp.setFValue();

                        //make a node* for queue
                        Tree::Node* t = tree->add(temp.getValue(), current);
                        //set function values
                        t->setGValue(temp.getGValue());
                        t->setHValue(temp.getHValue());
                        t->setFValue();

                        //push into queue
                        nodes.push(t);

                    }   //end if successor not an ancestor node
                }   //end for
            }   //end else not at goal
        }   //end else still positions to try
    }   //end while


    //trace back up the nodes to get the path
    while(!(current->getValue().equals(tree->getRoot()->getValue()))) {
        result.add(current->getValue());
        current = current->getParent();
    }   //end while

    //push root position
    result.add(current->getValue());

    //reverse the path vector so first position is vector.at(0), last position is vector.at(vector_length-1)
    result.reverse();
    delete tree;
    //std::cout<<"\nASTAR RETURNING:"<<result.toString();
    return result;
}   //END A*



Tree::Node* Grid_Analyzer::find_closest_node_in_tree(Tree*& tree, Position& ver) {
    Tree::Node* result = tree->getRoot();
    //loop through tree nodes, compare distance
    for(int i=0;i<tree->getNodes().size();i++) {
        if(getSLDistance(tree->getNodes().at(i)->getValue(), ver)
           < getSLDistance(result->getValue(), ver))
           result = tree->getNodes().at(i);
    }   //end for

    return result;
}   //END FIND_CLOSEST_IN_TREE


/*Gets surrounding positions of the position to form a group to pick a random sample from*/
std::vector<Position> Grid_Analyzer::get_potential_samples(Position& pos, int& rk, int& ck) {

    std::vector<Position> result;
    int startr;
    int startc;
    int endr;
    int endc;

    if(pos.getRow()-rk < 0)
        startr = 0;
    else
        startr = pos.getRow()-rk;

    if(pos.getCol()-ck < 0)
        startc = 0;
    else
        startc = pos.getCol() - ck;

    if(pos.getRow()+rk >= grid->getNumOfRows())
        endr = grid->getNumOfRows()-1;
    else
        endr = pos.getRow() + rk;

    if(pos.getCol()+ck >= grid->getNumOfCols())
        endc = grid->getNumOfCols()-1;
    else
        endc = grid->getNumOfCols()+ck;

    for(int r=startr;r<endr;r++) {
        for(int c=startc;c<endc;c++) {
            Position temp(r,c);
            result.push_back(temp);
        }
    }
    return result;
}   //END GET_POTENTIAL_SAMPLES


Path Grid_Analyzer::rrt_path(Position& init, Position& goal) {

    //make tree with pos as the init state
    Tree* tree = new Tree(init);

    Position x = init;
    int r_samp = 2;
    int c_samp = 2;
    srand(time(NULL));

    //while the goal is not found
    while(!tree->contains(goal)) {


        /*ANY RANDOM STATE!
        //xrand<-random_state
        //make a random position
        srand(rand());
        int row = (rand() % grid->getNumOfRows())+1;
        srand(rand());
        int col = (rand() % grid->getNumOfCols())+1;
        Position rand_state(row, col);
        //std::cout<<"\nrand_state: "<<rand_state.toString();*/


        //get list of potential samples
        std::vector<Position> potential_samples = get_potential_samples(init, r_samp, c_samp);
        //seed again with rand
        srand(rand());
        //get one of the samples
        Position rand_state = potential_samples.at(rand() % potential_samples.size());
        //std::cout<<"\nrand_state: "<<rand_state.toString()<<"\n";
        //increase r and c samp
        r_samp += r_samp;
        c_samp += c_samp;
        //avoid going over int max storage
        if(r_samp >= grid->getNumOfRows()) {
            r_samp = grid->getNumOfRows()-init.getRow()-1;
            c_samp = grid->getNumOfCols()-init.getCol()-1;
        }

        //if position is valid and not already in tree,
        //continue with algorithm
        if(positionValid(rand_state) && !tree->contains(rand_state)) {

            //xnear<-nearest_neighbor
            Tree::Node* near = find_closest_node_in_tree(tree, rand_state);
            //std::cout<<"\nfind_closest_node_in_tree returned:"<<near->getValue().toString()<<"\n";

            //get a path from nearest point to the random point
            Path connect_points = astar_path(near->getValue(), rand_state);

            //add the path to the tree
            for(int i=1;i<connect_points.getPathVector().size();i++)
                tree->add(connect_points.getPathVector().at(i), tree->find(connect_points.getPathVector().at(i-1)));

            //std::cout<<"\ntree:\n"<<tree->toString()<<"\n\n";
        }   //end if
    }   //end while


    Path result;
    Tree::Node* current = tree->find(goal);

    //trace back up the nodes to get the path
    while(!(current->getValue().equals(tree->getRoot()->getValue()))) {
        result.add(current->getValue());
        current = current->getParent();
    }

    result.add(current->getValue());
    result.reverse();

    //std::cout<<"\nrrt path from init:"<<init.toString()<<" to end:"<<end.toString()<<" :\n"<<result.toString();

    delete tree;
    return result;
}   //END RRT