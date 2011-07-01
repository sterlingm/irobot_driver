#include "priorityqueue.h"
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

/*Constructor, set count to 0*/
PriorityQueue::PriorityQueue() : count(0) {}

/*Destructor*/
PriorityQueue::~PriorityQueue() {}

/*Returns the top element of the queue*/
Tree::Node*& PriorityQueue::top() {return nodes.front();}

/*Returns the number of elements in the queue*/
int PriorityQueue::size() {return count;}

/*Returns true if there are no elements in the queue*/
bool PriorityQueue::isEmpty() {return count == 0;}

/*Returns true if n was found in the queue*/
bool PriorityQueue::contains(Tree::Node*& n) {
    for(int i=0;i<count;i++)
        if(nodes.at(i)->getValue().equals(n->getValue()))
            return true;
    return false;
}   //END CONTAINS

/*Returns the index n in the queue
  Throws a NodeNotFoundException if n is not in the queue*/
int PriorityQueue::findIndex(Tree::Node*& n) {
    for(int i=0;i<count;i++)
        if(nodes.at(i)->getValue().equals(n->getValue()))
            return i;

    throw NodeNotFoundException();
}   //END FINDINDEX

/*Adds n to the queue*/
void PriorityQueue::push(Tree::Node*& n) {

    //check if the node is already in the queue
    if(contains(n)) {
        //check if the position is in the queue and has a larger value than n
        //if so, remove it
        int i=findIndex(n);
        if(i > -1 && n->getFValue() < nodes.at(i)->getFValue())
            remove(nodes.at(i));
    }

    //if queue already has elements
    if(count>0) {

        int index = 0;  //index for new value to be added to

        //look at each element
        //add 1 to the index for each element less than n
        for(int i=0;i<count;i++) {
            if(n->getFValue() > nodes.at(i)->getFValue())
                index++;
            else
                break;
        }

        //if the index is the max index, just push back
        if(index == count)
            nodes.push_back(n);

        else {

            //set last value
            Tree::Node* temp = nodes.at(count-1);

            //rearrange queue
            for(int i=count-1;i>index;i--) {
                Tree::Node* t = nodes.at(i-1);
                nodes.at(i-1) = nodes.at(i);
                nodes.at(i) = t;
            }

            //set new value in pqueue
            nodes.at(index) = n;
            nodes.push_back(temp);
        }   //end else
    }   //end if count>0
    //else, just push back
    else
        nodes.push_back(n);

    //increment count
    count++;
}   //END PUSH

/*Removes the top element from the queue*/
void PriorityQueue::pop() {
    if(count == 0)
        throw EmptyQueueException();
    //set all values to the next element to remove the first element
    for(int i=0;i<count-1;i++)
        nodes.at(i) = nodes.at(i+1);
    //pop off duplicate in back
    nodes.pop_back();
    count--;
}   //END POP

/*Removes n from the queue*/
void PriorityQueue::remove(Tree::Node*& n) {
    int index = findIndex(n);
    if(index == -1)
        throw NodeNotFoundException();

    //swap last element with the index and pop_back
    Tree::Node* temp = nodes.at(index);
    nodes.at(index) = nodes.back();
    nodes.at(nodes.size()-1) = temp;
    nodes.pop_back();
    count--;

    //put queue back in order
    for(int i=index;i<count-1;i++) {
        Tree::Node* temp = nodes.at(i);
        nodes.at(i) = nodes.at(i+1);
        nodes.at(i+1) = temp;
    }
}   //END REMOVE

/*Removes all elements from the queue*/
void PriorityQueue::clear() {
    for(int i=0;i<count;i++)
        nodes.pop_back();
    count = 0;
}   //END CLEAR

/*Reutrns a string listing the queues elements in order*/
std::string PriorityQueue::toString() {
    std::ostringstream result;
    result<<"\n";
    for(int i=0;i<count;i++)
        result<<nodes.at(i)->getValue().toString();
    return result.str();
}   //END TOSTRING


