
#include "robot_driver_priorityqueue.h"
#include <sstream>
#include <stdio.h>


/*Exception for trying to pop off the queue when it is empty*/
const char* PriorityQueue::EmptyQueueException::what() const throw() {
	return "The queue is empty!\n";
}
/*Exception for trying to find an index of a node not in the queue*/
const char* PriorityQueue::NodeNotFoundException::what() const throw() {
	return "That node was not found in the queue!\n";
}


PriorityQueue::PriorityQueue() {nodes = new Tree::Node*[50]; max_size = 50; count = 0;}
PriorityQueue::PriorityQueue(int& size) {nodes = new Tree::Node*[size]; max_size = size; count = 0;}
PriorityQueue::~PriorityQueue() {delete [] nodes; nodes = 0;}



/*Returns the top element of the queue*/
Tree::Node*& PriorityQueue::top() {return nodes[0];}

/*Returns the number of elements in the queue*/
int PriorityQueue::size() {return count;}

/*Returns true if there are no elements in the queue*/
bool PriorityQueue::isEmpty() {return count == 0;}



/*Adds n to the queue*/
void PriorityQueue::push(Tree::Node*& n) {


    //check if the node is already in the queue
    if(contains(n)) {
        //check if the position in the queue has a larger value than n
        //if so, remove it before adding n
        int i=findIndex(n);
        if(i > -1 && n->getFValue() < nodes[i]->getFValue())
            remove(nodes[i]);
    }   //end if node is already in queue


    //if queue already has elements
    if(count>0) {

        //check if we need to expand the queue size
        if(count == max_size)
            grow();

        //index for new value to be added to
        int index = 0;

        //find right index - look at each element and add 1 to the index for each element less than n
        for(int i=0;i<count;i++) {
            if(n->getFValue() > nodes[i]->getFValue())
                index++;
            else
                break;
        }   //end for

        //if the index is the max index, just push back
        if(index == count)
            nodes[count] = n;

        //else, put n at specific index
        else {

            //set last value
            Tree::Node* temp = nodes[count-1];

            //rearrange queue
            for(int i=count;i>index;i--) {
                Tree::Node* t = nodes[i-1];
                nodes[i-1] = nodes[i];
                nodes[i] = t;
            }   //end for

            //set new value in pqueue
            nodes[index] = n;
        }   //end else
    }   //end if count>0

    //else, just push back
    else
        nodes[0] = n;

    //increment count
    count++;
}   //END PUSH

/*Returns the index n in the queue
  Throws a NodeNotFoundException if n is not in the queue*/
int PriorityQueue::findIndex(Tree::Node*& n) {
    for(int i=0;i<count;i++)
        if(nodes[i]->getValue().equals(n->getValue()))
            return i;

    throw NodeNotFoundException();
}   //END FINDINDEX


/*Returns true if n was found in the queue*/
bool PriorityQueue::contains(Tree::Node*& n) {
    for(int i=0;i<count;i++)
        if(nodes[i]->getValue().equals(n->getValue()))
            return true;
    return false;
}   //END CONTAINS

/*Removes the top element from the queue*/
void PriorityQueue::pop() {
    if(count == 0)
        throw EmptyQueueException();

    //set all values to the next element to remove the first element
    for(int i=0;i<count-1;i++)
        nodes[i] = nodes[i+1];

    //pop off duplicate in back
    nodes[count-1] = NULL;
    count--;
}   //END POP

/*Removes n from the queue*/
void PriorityQueue::remove(Tree::Node*& n) {
    int index = findIndex(n);
    if(index == -1)
        throw NodeNotFoundException();

    //swap last element with the index and pop_back
    Tree::Node* temp = nodes[index];
    nodes[index] = nodes[count-1];
    nodes[count-1] = temp;
    nodes[count-1] = NULL;
    count--;

    //put queue back in order
    for(int i=index;i<count-1;i++) {
        Tree::Node* temp = nodes[i];
        nodes[i] = nodes[i+1];
        nodes[i+1] = temp;
    }   //end for
}   //END REMOVE




/*Removes all elements from the queue*/
void PriorityQueue::clear() {
    for(int i=count-1;i>0;i--)
        nodes[i] = NULL;
    count = 0;
}   //END CLEAR


void PriorityQueue::grow() {

    Tree::Node* temp[max_size];
    for(int i=0;i<max_size;i++)
        temp[i] = nodes[i];
    int temp_max = max_size;
    max_size = max_size * 2;

    delete [] nodes;
    nodes = new Tree::Node*[max_size];

    for(int i=0;i<temp_max;i++)
        nodes[i] = temp[i];

}



/*Reutrns a string listing the queues elements in order*/
const std::string PriorityQueue::toString() {
    std::ostringstream result;
    result<<"\n";
    for(int i=0;i<count;i++)
        result<<nodes[i]->getValue().toString()<<" ";
    return result.str();
}   //END TOSTRING
