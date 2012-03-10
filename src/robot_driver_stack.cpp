#include "robot_driver_stack.h"



const char* Stack::EmptyTreeException::what() const throw() {
	return "The stack is empty!\n";
}

//constructor
Stack::Stack() : count(0), top(0) {}


//delete every node if not empty
Stack::~Stack() {
	if(!isEmpty()) {
		Node *nodePtr = top;	//start at top

		while(nodePtr != 0) {
			Node* temp = nodePtr->next;
			delete nodePtr;
			nodePtr = temp;
		}	//end while
	}
}	//END DESTRUCTOR


Position Stack::peek() {return top->value;}

bool Stack::isEmpty() {return (count == 0);}

int Stack::size() {return count;}



//push a value onto the stack
void Stack::push(Position& p) {
	Node* temp = new Node;
	temp->value = p;

	//if empty, make value the top
	if(isEmpty()) {
		top = temp;
		temp->next = 0;
	}
	//else add to top
	else {
		temp->next = top;
		top = temp;
	}

	count++;
}	//END PUSH


//pop a value off of the stack and return it
Position Stack::pop() {
	if(isEmpty())
		throw EmptyTreeException();

	Position result = top->value;

	Node* temp = top->next;
	delete top;
	top = temp;

	count--;
	return result;
}	//END POP
