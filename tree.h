#ifndef TREE_H
#define TREE_H
#include <vector>
#include "position.h"


class Tree {

public:

	//node class
	//nodes should have value, their parent, and all of their children
	//and functions to get all of those
	class Node {

	public:
		//when declaring a node, give it
		//a pos value nd a parent vnode
		Node();
		Node(Position&, Node*&);
		~Node();

		Node clone(Node*&);

		bool isLeaf();

		Position& getValue();
		Node*& getParent();
		std::vector<Node*>& getChildren();
        double getGValue();
        double getHValue();
        double getFValue();
		void setValue(Position&);
        void setParent(Node*&);
        void setGValue(double);
        void setHValue(double);
        void setFValue();

		std::string toString();

	private:
		double gvalue;
		double hvalue;
		double fvalue;

		Position value;
		Node* parent;
		std::vector<Node*> children;
	};	//END NODE CLASS


	class NodeNotFoundException : public std::exception {
	public:
		virtual const char* what() const throw();
	};	//end exception


	Tree();
	Tree(Position&);
	~Tree();

	Node*& getRoot();

    bool contains(Position&);
    Node*& find(Node*&);

	Node* add(Position&, Node*&);
	void addGroup(std::vector<Position>&, Node*&);

	std::string rootToString();
	std::string toString(Node*&);
	std::string toString();
private:

	//root of tree
	Node* dummyroot;
	Node* root;

	std::vector<Node*> nodes;

};

#endif
