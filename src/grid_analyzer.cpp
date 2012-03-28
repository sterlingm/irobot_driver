#include "grid_analyzer.h"
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

Grid_Analyzer::Grid_Analyzer() : grid(0) {}
Grid_Analyzer::Grid_Analyzer(Grid* g) : grid(g) {}
Grid_Analyzer::~Grid_Analyzer() {}


Grid*& Grid_Analyzer::getGrid() {return grid;}
void Grid_Analyzer::setGrid(Grid* g) {grid = g;}

/*Returns the straight line distance between two positions*/
double Grid_Analyzer::getEuclideanDistance(Position& a, Position& b) {
    return sqrt( pow(a.getRow()-b.getRow(), 2) + pow(a.getCol()-b.getCol(),2) );
}   //END getEuclideanDistance


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
        if( getEuclideanDistance(tree.getNodes().at(i)->getValue(), e) <
                getEuclideanDistance(nb->getValue(), e) )
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
    tree->getRoot()->setHValue(getEuclideanDistance(init, goal));
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
                        temp.setHValue(getEuclideanDistance(temp.getValue(), goal));
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



Path Grid_Analyzer::astar_path_decomposed(Position& init, Position& goal, Tree* tree) {
    //std::cout<<"\nDECOMPOSED TREE:\n"<<tree->toString();
    //make path to return
    Path result;

    //the parent to current for robot tracing
    Tree::Node* parent = tree->getRoot();
    //current node
    Tree::Node* current = tree->getRoot();
    //list of nodes
    PriorityQueue nodes;

    //push on current
    nodes.push(current);

    //flag to stop
    bool done = false;
    //while not done
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

            //set current to the top
            current = nodes.top();

            //pop off node
            nodes.pop();

            //if we are at the goal, end loop
            if(current->getValue().equals(goal))
                done = true;

            //else, traverse further
            else {
                //get child nodes for current
                std::vector<Tree::Node*> successors = current->getChildren();
                //std::cout<<"\nPositions adjacent to "<<current->getValue().toString();
                //for(int i=0;i<successors.size();i++)
                    //std::cout<<"\n"<<successors.at(i).toString();

                //push on all successors
                for(int i=0;i<successors.size();i++) {
                    //push into queue
                    nodes.push(successors.at(i));
                }   //end for
            }   //end else traversre further
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

    return result;
}   //END A*_DECOMPOSED



Tree::Node* Grid_Analyzer::find_nearest_neighbor(Tree*& tree, Position& ver) {
    Tree::Node* result = tree->getRoot();
    //loop through tree nodes, compare distance
    for(int i=0;i<tree->getNodes().size();i++) {
        if(getEuclideanDistance(tree->getNodes().at(i)->getValue(), ver)
           < getEuclideanDistance(result->getValue(), ver))
           result = tree->getNodes().at(i);
    }   //end for

    return result;
}   //END FIND_NEAREST_NEIGHBOR


std::vector<Position>* Grid_Analyzer::get_sampling_sets(Position& init, Position& goal) {
    std::vector<Position>* result = new std::vector<Position>[2];
    int row_diff = init.getRow() - goal.getRow();
    int col_diff = init.getCol() - goal.getCol();

    int startr = (row_diff > 0) ? goal.getRow() : init.getRow();
    int startc = (col_diff > 0) ? goal.getCol() : init.getCol();
    int endr = (row_diff > 0) ? init.getRow() : goal.getRow();
    int endc = (col_diff > 0) ? init.getCol() : goal.getCol();

//    std::cout<<"\nstartr:"<<startr;
//    std::cout<<"\nstartc:"<<startc;
//    std::cout<<"\nendr:"<<endr;
//    std::cout<<"\nendc:"<<endc;


    for(int r=0;r<grid->getNumOfRows();r++) {
        for(int c=0;c<grid->getNumOfCols();c++) {
            Position temp(r,c);
            if( (endr - temp.getRow() >= 0) && (endc - temp.getCol() >= 0)
               && (temp.getRow() - startr >= 0) && (temp.getCol() - startc >= 0) )
                result[0].push_back(temp);

            else
                result[1].push_back(temp);

        }
    }

    return result;
}


//greedy search based on euclidean distance
Path Grid_Analyzer::connect_for_rrt(Position& near_neigh, Position& sample) {
    //std::cout<<"\nnn:"<<near_neigh.toString()<<" s:"<<sample.toString();

    Path result;
    Position current=near_neigh;
    Stack s;
    s.push(near_neigh);
    std::vector<Position> local;
    bool done=false;

    //grid->setPos(sample.getRow(), sample.getCol(), ' ');
    //get adjacent pos
    //push on one with lowest sld
    while(!done) {

        //if stack is not empty
        if(!s.isEmpty()) {
            current = s.pop();
            result.add(current);
            grid->setPos(current.getRow(),current.getCol(),'.');
            //std::cout<<"\ncurrent:"<<current.toString();

            //if at the sample, done
            if(current.equals(sample))
                done = true;

            else {
                local = adjacentPositions(current);
                //std::cout<<"\nadjacent pos to "<<current.toString()<<":";
                //for(int i=0;i<local.size();i++)
                    //std::cout<<" "<<local.at(i).toString();
                if(local.size() > 0) {
                    int min_index=0;
                    for(int i=1;i<local.size();i++) {
                        if(getEuclideanDistance(local.at(i), sample) < getEuclideanDistance(local.at(min_index), sample)) {
                            //std::cout<<"\n"<<local.at(i).toString()<<":"<<getEuclideanDistance(local.at(i), sample);
                            //std::cout<<"\n"<<local.at(min_index).toString()<<":"<<getEuclideanDistance(local.at(min_index), sample);
                            min_index = i;
                        }
                    }   //end for
                    //std::cout<<"\n"<<local.at(min_index).toString()<<" is closest";
                    s.push(local.at(min_index));
                }   //end if local.size>0
            }   //end else
        }   //end if stack not empty
        //if stack is empty, cannot reach goal
        else
            done = true;
    }   //end while
    grid->clear();

    //std::cout<<"\nresult:"<<result.toString()<<"\n";
    return result;
}   //END CONNECT_FOR_RRT


/*RESULT IS ALLOCATED ON HEAP!!*/
Tree* Grid_Analyzer::build_rrt(Position& init, Position& goal) {

    //make tree with pos as the init state
    Tree* tree = new Tree(init);

    //get the sampling box
    std::vector<Position>* sampling_sets = get_sampling_sets(init, goal);
    int set = 0;
//
//    std::cout<<"\nset:"<<set;
//    for(int i=0;i<sampling_sets[set].size();i++)
//        std::cout<<" "<<sampling_sets[set].at(i).toString();
//    std::cout<<"\nset:"<<set+1;
//    for(int i=0;i<sampling_sets[set+1].size();i++)
//        std::cout<<" "<<sampling_sets[set+1].at(i).toString();

    //make Position to hold the sample
    Position sampled_state;

    //how many Positions we sample with each iteration
    int branching = 5;

    //if we sample within 30% of grid bounds to the goal, stop and connect
    float close_distance = grid->getNumOfCols() * 0.3;
    //std::cout<<"\nclose_distance:"<<close_distance;

    //holds the index of sample in sampling_boxes[box]
    int index;

    //seed rand
    srand(time(NULL));

    //flag for done
    bool done_sampling=false;
    //while the goal is not found
    while(!done_sampling) {

        //get number of samples to add equal to branching
        for(int i=0;i<branching;i++) {
            //std::cout<<"\nsampling_boxes["<<box<<"] size:"<<sampling_boxes[box].size();


            if(sampling_sets[set].size() == 1) {
                //std::cout<<"\nNO PATH IN sampling_boxes[0]";
                if(set == 0)
                    set++;
                else
                    i = branching;
            }

            else {

                //seed again with rand
                srand(rand());

                //get an index
                index = rand() % sampling_sets[set].size();

                //get one of the samples
                sampled_state = sampling_sets[set].at(index);
                //std::cout<<"\nsampled_state: "<<sampled_state.toString()<<"\n";

                //if position is valid
                if(positionValid(sampled_state)) {
                    //xnear<-nearest_neighbor
                    Tree::Node* near = find_nearest_neighbor(tree, sampled_state);
                    //std::cout<<"\nfind_closest_node_in_tree returned:"<<near->getValue().toString()<<"for "<<sampled_state.toString()<<"\n";

                    //get a path from nearest point to the random point
                    Path connect_points = connect_for_rrt(near->getValue(), sampled_state);

                    //add the path to the tree
                    for(int j=1;j<connect_points.getPathVector().size();j++) {
                        Tree::Node* n = tree->find(connect_points.getPathVector().at(j-1));
                        tree->add(connect_points.getPathVector().at(j), n);
                    }
                    //std::cout<<"\ntree:\n"<<tree->toString()<<"\n\n";



                    //if we've sampled the goal or are close enough, break out of loops
                    if(sampled_state.equals(goal) || (sqrt( pow( (goal.getRow() - sampled_state.getRow()), 2)
                                                            + pow( (goal.getCol() - sampled_state.getCol()), 2)) <= close_distance)) {
                    //if(sampled_state.equals(goal) || (abs(goal.getRow() - sampled_state.getRow()) <= close_enough_r)
                                                //|| (abs(goal.getCol() - sampled_state.getCol()) <= close_enough_c) ) {
                        done_sampling = true;
                        i = branching;
                    }   //end if sampled goal or close enough
                }   //end if valid position

                //remove position from sample list
                sampling_sets[set].erase(sampling_sets[set].begin()+index);
            }   //end else
        }   //end for
    }   //end while

    //std::cout<<"\noutide of while with rand_state:"<<sampled_state.toString()<<": "<<grid->getPos(sampled_state.getRow(), sampled_state.getCol())<<"\n";

    //if stopped because close to goal
    if(!tree->contains(goal)) {
        //std::cout<<"\nstopped because close enough to goal at "<<sampled_state.toString();
        //std::cout<<"\ndistance:"<<sqrt( pow( (goal.getRow() - sampled_state.getRow()), 2)
                                                    //+ pow( (goal.getCol() - sampled_state.getCol()), 2));
        //xnear<-nearest_neighbor
        Tree::Node* near = find_nearest_neighbor(tree, goal);
        //std::cout<<"\nfind_closest_node_in_tree returned:"<<near->getValue().toString()<<"for "<<rand_state.toString()<<"\n";

        //get a path from nearest point to the random point
        Path connect_points = connect_for_rrt(near->getValue(), goal);
        //std::cout<<"\nconnect_points:"<<connect_points.toString()<<"\n";

        //add the path to the tree
        for(int i=1;i<connect_points.getPathVector().size();i++) {
            Tree::Node* n = tree->find(connect_points.getPathVector().at(i-1));
            tree->add(connect_points.getPathVector().at(i), n);
        }   //end for
    }   //end if stopped because close to goal

    delete [] sampling_sets;
    return tree;
}   //END BUILD_RRT

Path Grid_Analyzer::rrt_path(Position& init, Position& goal) {
    //std::cout<<"\ninit:"<<init.toString()<<" end:"<<end.toString()<<"\n";
    //std::cout<<"\ngrid:\n"<<grid->toString();
    //std::cout<<"\ngrid rows:"<<grid->getNumOfRows()<<" grid columns:"<<grid->getNumOfCols();

    //value to return
    Path result;

    Tree* tree = build_rrt(init, goal);
    //std::cout<<"\ntree:\n"<<tree->toString();

    //if tree does not contain the goal,
    //there is no path to the goal.
    //find the closest we can get to the goal and set that as goal
    if(!tree->contains(goal)) {

        //call find_next_best
        Position temp = find_next_best(*tree, goal);

        //std::cout<<"\nNO PATH - SETTING GOAL TO "<<temp.toString()<<"\n";
        //set end to the next best position
        goal = temp;
    }   //end if

    //hold current
    Tree::Node* current = tree->find(goal);

    //trace back up the nodes to get the path
    while(!(current->getValue().equals(tree->getRoot()->getValue()))) {
        result.add(current->getValue());
        current = current->getParent();
    }

    //add the root
    result.add(current->getValue());
    //reverse
    result.reverse();

    //std::cout<<"\nrrt path from init:"<<init.toString()<<" to end:"<<end.toString()<<" :\n"<<result.toString()<<"\n";
    //delete [] sampling_sets;
    delete tree;
    return result;
}   //END RRT
