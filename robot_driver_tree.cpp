#include <sstream>
#include <stdlib.h>
#include "robot_driver_tree.h"



/*Constructor*/
Tree::Node::Node(Position& v, Node*& p) : value(v), parent(p), gvalue(0), hvalue(0), fvalue(0) {}


/*Destructor*/
Tree::Node::~Node() {}

Tree::Node Tree::Node::clone() {return *this;}


/*Returns true if the node has no children*/
bool Tree::Node::isLeaf() {return getChildren().size() == 0;}

/*Returns value*/
Position& Tree::Node::getValue() {return value;}
/*Returns parent*/
Tree::Node*& Tree::Node::getParent() {return parent;}
/*Returns children*/
std::vector<Tree::Node*>& Tree::Node::getChildren() {return children;}
/*Returns gvalue*/
double Tree::Node::getGValue() {return gvalue;}
/*Returns hvalue*/
double Tree::Node::getHValue() {return hvalue;}
/*Returns fvalue*/
double Tree::Node::getFValue() {return fvalue;}

/*Sets parent to p*/
void Tree::Node::setParent(Node*& p) {parent = p;}
/*Sets value to v*/
void Tree::Node::setValue(Position& v) {value = v;}
/*Sets gvalue to v*/
void Tree::Node::setGValue(double v) {gvalue = v;}
/*Sets hvalue to v*/
void Tree::Node::setHValue(double v) {hvalue = v;}
/*Sets fvalue to v*/
void Tree::Node::setFValue() {fvalue = gvalue + hvalue;}



/*Returns a string of the Node's value, it's parent, and it's children*/
std::string Tree::Node::toString() {
	std::ostringstream result;
	result<<getValue().toString()<<" ("<<this<<")";
	result<<"		Parent - "<<getParent()->getValue().toString()<<"		Children - ";

	for(int i=0;i<children.size();i++) {
		if(i==0)
			result<<children.at(i)->getValue().toString();
		else
			result<<", "<<children.at(i)->getValue().toString();
	}	//end for
	return result.str();
}   //END NODE::TOSTRING




/*Exception for when a node is not found in the tree*/
const char* Tree::NodeNotFoundException::what() const throw() {
	return "The node is not in the tree!\n";
}


/*Constructor that sets p as the root value*/
Tree::Tree(Position& p) {
	Node* temp = new Node(p, dummyroot);
	root = temp;
	nodes.push_back(temp);
}   //END CONSTRUCTOR

/*Destructor*/
Tree::~Tree() {
    for(int i=0;i<nodes.size();i++) {
        delete nodes.at(i);
        nodes.at(i) = 0;
    }
}

/*Returns root*/
Tree::Node*& Tree::getRoot() {return root;}

std::vector<Tree::Node*> Tree::getNodes() {return nodes;}

/*Returns true if p is in the tree*/
bool Tree::contains(Position& p) {
    for(int i=0;i<nodes.size();i++)
        if(p.equals(nodes.at(i)->getValue()))
            return true;
    return false;
}   //END CONTAINS


Tree::Node* Tree::add(Position& value, Node*& parent) {
	Node* temp = new Node(value, parent);
	parent->getChildren().push_back(temp);
	//std::cout<<"\n"<<temp->getValue().toString()<<" added to "<<parent->getValue().toString();
	nodes.push_back(temp);
    return temp;
}   //END ADD

/*Returns a string of the root node and it's children*/
const std::string Tree::rootToString() {
	std::ostringstream result;

	result<<getRoot()->getValue().toString()<<" ("<<getRoot()<<") "<<"          (ROOT NODE)";
	result<<"       	Children - ";

	//print children
	for(int i=0;i<getRoot()->getChildren().size();i++) {
		if(i==0)
			result<<getRoot()->getChildren().at(i)->getValue().toString();
		else
			result<<", "<<getRoot()->getChildren().at(i)->getValue().toString();
	}	//end for

	return result.str();
}	//END ROOT TOSTRING


/*Returns a string of the tree values from t down*/
const std::string Tree::toString(Node*& t) {

	std::ostringstream result;

	//if null, print empty line
	if(t == NULL)
		return "()\n";

    result<<"\n";
	if(t == getRoot())
		result<<rootToString();
	else
		result<<t->toString();

	//if its not a leaf, print the children
	if(!t->isLeaf()) {

		std::vector<Node*> ctemp = t->getChildren();

		while(ctemp.size() > 0) {
			result<<"\n";

			//pop off a child and recurse
			Node* temp = ctemp.front();
			ctemp.erase(ctemp.begin());
			result<<toString(temp);

		}	//end while
	}	//end if not a leaf
	return result.str();
}   //END TOSTRING

/*Returns a string of the whole tree*/
const std::string Tree::toString() {
    return toString(root);
}   //END TOSTRING



Tree::Node* Tree::find(Position& a) {

    for(int i=0;i<nodes.size();i++)
        if(nodes.at(i)->getValue().equals(a))
            return nodes.at(i);

}   //END FIND



