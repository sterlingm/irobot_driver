#ifndef STACK_H
#define STACK_H
#include "robot_driver_position.h"
class Stack {
public:

	class EmptyTreeException : public std::exception {
	public:
		virtual const char* what() const throw();
	};	//end exception

	Stack();
	~Stack();

	Position peek();
	bool isEmpty();
	int size();

	void push(Position&);
	Position pop();

private:

	//struct for the nodes
	struct Node {
		Position value;
		Node* next;	//point downward to next node
	};	//end struct node

	Node* top;
	int count;
};

#endif
