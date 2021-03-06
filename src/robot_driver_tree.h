#ifndef ROBOT_DRIVER_TREE_H
#define ROBOT_DRIVER_TREE_H
#include <vector>
#include "robot_driver_position.h"


class Tree {

public:

	class Node {

	public:

        //! A Constructor
        /*! Sets value to v and parent to p */
		Node(Position&, Node*&);
		//! A Destructor
		~Node();

        //! Clones a Node
        /*! Returns an instance of this object */
		Node clone();

        //! Check if Node is a leaf
        /*! Returns true if Node has zero children */
		bool isLeaf();

        //! Getter function for value member
        /*! Returns a reference of the value member */
		Position& getValue();

        //! Setter function for value member
        /*! Sets value to v */
		void setValue(Position&);

        //! Getter function for parent member
        /*! Returns a reference of the parent member */
		Node*& getParent();

        //! Setter function for parent member
        /*! Sets parent to p */
        void setParent(Node*&);

        //! Getter function for gvalue member
        /*! Returns value of gvalue member */
        double getGValue();

        //! Setter for gvalue member
        /*! Sets gvalue to v */
        void setGValue(double);

        //! Getter function for hvalue member
        /*! Returns value of hvalue member */
        double getHValue();

        //! Setter function for hvalue member
        /*! Returns value of hvalue member */
        void setHValue(double);

        //! Getter function for fvalue member
        /*! Returns value of fvalue member */
        double getFValue();

        //! Setter function for fvalue member
        /*! Sets fvalue to v */
        void setFValue();

        //! Getter function for children member
        /*! Returns reference of the children member */
		std::vector<Node*>& getChildren();

        //! Returns a printable string of the Node
		std::string toString();

	private:
		double gvalue;
		double hvalue;
		double fvalue;

		Position value;
		Node* parent;
		std::vector<Node*> children;
	};	//END NODE CLASS

    //! Exception class for when a Node is not found
	class NodeNotFoundException : public std::exception {
	public:
		virtual const char* what() const throw();
	};	//end exception


    //! A Constructor
    /*! Sets root to p */
	explicit Tree(Position&);
	//! A Destructor
	~Tree();

    //! Getter function for root member
    /*! Returns a reference of root member */
	Node*& getRoot();

	std::vector<Node*> getNodes();

    //! Checks if a Position is in the tree
    /*! Returns true if a Position in the tree is equal to p */
    bool contains(Position&);

    Tree::Node*& find(Position&);

    //! Add a Position to the tree
    /*! Makes a node for value and adds that node to the tree as one of parent's children */
	Node* add(Position&, Node*&);

	//! Returns a printable string of the while tree
	const std::string toString();
private:

	//root of tree
	Node* dummyroot;
	Node* root;
	std::vector<Node*> nodes;

	const std::string toString(Node*&);
	const std::string rootToString();
};

#endif
