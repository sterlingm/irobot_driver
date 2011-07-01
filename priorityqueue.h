#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE
#include <string>
#include "tree.h"

class PriorityQueue {

public:

	class EmptyQueueException : public std::exception {
	public:
		virtual const char* what() const throw();
	};	//end exception

    class NodeNotFoundException : public std::exception {
	public:
		virtual const char* what() const throw();
	};	//end exception

    PriorityQueue();
    ~PriorityQueue();


    Tree::Node*& top();
    int size();
    bool isEmpty();
    bool contains(Tree::Node*&);

    void push(Tree::Node*&);
    void pop();
    void remove(Tree::Node*&);


    void clear();

    std::string toString();

private:
    int findIndex(Tree::Node*&);
    std::vector<Tree::Node*> nodes;
    int count;

};
#endif
