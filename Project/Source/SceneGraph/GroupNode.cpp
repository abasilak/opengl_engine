//----------------------------------------------------//
//                                                    //
// File: GroupNode.cpp								  //
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

// includes ////////////////////////////////////////
#include "../HelpLib.h"	   // - Library for including GL libraries, checking for OpenGL errors, writing to Output window, etc.
#include "GroupNode.h"	   // - Header file for the GroupNode class

// defines /////////////////////////////////////////


// Constructor
GroupNode::GroupNode(const char* name):
Node(name)
{
	children.clear();
}

// Destructor
GroupNode::~GroupNode()
{
	for (unsigned int i=0; i<children.size();i++)
	{
		Node* nd = children.at(i); 
		//SAFE_DELETE(nd);
	}
	children.clear();
}

// other functions
void GroupNode::AddChild(Node *nd)
{
	nd->SetParent(this);
	children.push_back(nd);
}

void GroupNode::RemoveChild(Node *nd)
{
	for (std::vector<Node*>::iterator iter = children.begin(); iter!=children.end() ; iter++ )
	{
		if (nd==*iter)
		{
			GroupNode* group_node = dynamic_cast<GroupNode*>(nd);
			if (group_node)
			{
				RemoveChildren(*group_node);
			}

			Print("Removed node %s from parent %s", (*iter)->GetName(), this->GetName());
			SAFE_DELETE(*iter);
			children.erase(iter);
			break;
		}
	}
}

void GroupNode::RemoveChildren(GroupNode &gd)
{
	for (std::vector<Node*>::iterator iter = gd.children.begin(); iter != gd.children.end() ; iter++ )
	{
		GroupNode* group_node = dynamic_cast<GroupNode*>(*iter);
		if (group_node)
		{
			RemoveChildren(*group_node);
		}
		Print("Removed node %s from parent %s", (*iter)->GetName(), gd.GetName());
		SAFE_DELETE(*iter);
	}
	gd.children.clear();
}

void GroupNode::Update()
{
	Node::Update();

	for (unsigned int i=0; i<children.size();i++)
		children.at(i)->Update();
}

void GroupNode::Draw(int shader_type)
{
	for (unsigned int i=0; i<children.size();i++)
		children.at(i)->Draw(shader_type);
}

void GroupNode::Init()
{
	for (unsigned int i=0; i<children.size();i++)
		children.at(i)->Init();
	Node::Init();
}

Node *GroupNode::GetNodeByName(const char *node_name)
{
	Node *nd = NULL;
	GroupNode *group = NULL;
	if (!node_name)
		return NULL;
	for (unsigned int i=0; i<children.size();i++)
	{
		nd = children.at(i);
		const char* name= nd->GetName();
		if (strcmp(node_name,name) != 0)
		{
			return nd;
		}
		group = dynamic_cast<GroupNode*>(nd);
		if (group)
		{
			nd = group->GetNodeByName(node_name);
			if (nd)
				return nd;
		}
	}
	return NULL;
}

// eof ///////////////////////////////// class GroupNode