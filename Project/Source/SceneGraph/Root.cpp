//----------------------------------------------------//
//                                                    //
// File: Root.h										  //
// This scene graph is a basic example for the		  //
// object relational management of the scene          //
// This holds the root node							  //
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
#include "Root.h"		   // - Header file for the Root class

// defines /////////////////////////////////////////

void Root::Draw(int shader_type)
{
	GroupNode::Draw(shader_type);
}

// Constructor
Root::Root():
GroupNode("root")
{
	m_parent = nullptr;
	m_root = this;
	m_basic_geometry_shader = nullptr;
	m_spotlight_shader = nullptr;
	m_shadow_map_shader = nullptr;
	m_gbuffer_shader = nullptr;

	m_world_transform = new TransformNode(m_name.c_str());
}

// Destructor
Root::~Root()
{
	SAFE_DELETE (m_world_transform);
}

// other functions
void Root::Update()
{
	GroupNode::Update();
}
	
void Root::Init()
{
	GroupNode::Init();
	SetRoot(this);
}

void Root::SetRoot(GroupNode* gnd)
{
	for (unsigned int i=0; i<gnd->children.size();i++)
	{
		Node* childnode = gnd->children.at(i);
		childnode->SetWorld(this);

		GroupNode* group = dynamic_cast<GroupNode*>(childnode);
		if (group)
		{
			if (group->children.size() > 0)
			{
				SetRoot(group);
			}
		}
	}
}

// eof ///////////////////////////////// class Root