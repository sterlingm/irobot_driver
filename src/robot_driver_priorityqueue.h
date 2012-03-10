
#ifndef ROBOT_DRIVER_PRIORITYQUEUE_H
#define ROBOT_DRIVER_PRIORITYQUEUE
#include <string>
#include "robot_driver_tree.h"


class PriorityQueue {

public:

    //! Exception for when the queue is empty
	class EmptyQueueException : public std::exception {
	public:
		virtual const char* what() const throw();
	};	//end exception

	//! Exception for when a node is not found
    class NodeNotFoundException : public std::exception {
	public:
		virtual const char* what() const throw();
	};	//end exception

	//! Constructor
	/*! Defaults the max_size to 50 */
    PriorityQueue();
    //! Constructor
    /*! Sets max_size to size */
    explicit PriorityQueue(int&);
    //! A Destructor
    ~PriorityQueue();

	//! Get the top of the queue
	/*! Returns the first node in the queue */
    Tree::Node*& top();

    //! Get the size of the queue
	/*! Returns the number of nodes in the queue */
    int size();

	//! Check if the queue is empty
	/*! Returns true if the queue has zero nodes */
    bool isEmpty();

	//! Check if the queue contains a node
	/*! Returns true if there is a node in the queue equal to n */
    bool contains(Tree::Node*&);

	//! Push a node onto the queue
	/*! Pushes n onto the back of the queue */
    void push(Tree::Node*&);

	//! Pop a node off of the queue
	/*! Removes a node off the front of the queue */
    void pop();

    //! Remove a node from the queue
	/*! Remove n from the queue */
    void remove(Tree::Node*&);

	//! Clear the queue
    void clear();

	//! Returns a printable string of the queue
    const std::string toString();

private:
    int findIndex(Tree::Node*&);
    void grow();

    Tree::Node** nodes;
    int count;
    int max_size;
};
#endif
