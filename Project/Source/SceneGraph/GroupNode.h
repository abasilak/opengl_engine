//----------------------------------------------------//
//                                                    //
// File: GroupNode.h								  //
// This scene graph is a basic example for the		  //
// object relational management of the scene          //
// This holds the group node which has a one to many  //
// relationship with its children nodes				  //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//
#ifndef GROUPNODE_H
#define GROUPNODE_H

#pragma once
//using namespace

// includes ////////////////////////////////////////
#include "Node.h"

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class GroupNode: public Node
{
protected:
	// protected variable declarations


	// protected function declarations

private:
	// private variable declarations


	// private function declarations


public:
	// Constructor
	GroupNode(const char* name);

	// Destructor
	~GroupNode(void);

	// public function declarations
	virtual void						Init(void);
	virtual void						Update(void);
	virtual void						Draw(int shader_type);

	virtual void						AddChild(Node *nd);
	virtual void						RemoveChild(Node *nd);		
	Node *								GetNodeByName(const char* node_name);
	virtual void						RemoveChildren(GroupNode &gd);
	std::vector<Node*>					children;

	// get functions


	// set functions


};

#endif //GROUPNODE_H

// eof ///////////////////////////////// class GroupNode