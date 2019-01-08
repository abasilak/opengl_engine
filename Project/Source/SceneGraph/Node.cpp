//----------------------------------------------------//
//                                                    //
// File: Node.cpp									  //
// This scene graph is a basic example for the		  //
// object relational management of the scene          //
// This is the basic node class						  //
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
#include "Node.h"		   // - Header file for the Node class

// defines /////////////////////////////////////////


// Constructor
Node::Node(const char* name):
m_name(name),
m_parent(nullptr)
{

}

// Destructor
Node::~Node()
{		

}

void Node::Init()
{

}

void Node::Update()
{

}

void Node::Draw(int shader_type)
{

}

void Node::SetName(const char * str)
{
	if (!str)
		return;

	m_name = std::string(str);
}

const char * Node::GetName()
{
	return m_name.c_str();
}

glm::mat4x4 Node::GetTransform()
{
	if (m_parent)
		return m_parent->GetTransform();
	else
		return glm::mat4x4(1);
}

glm::mat4x4	Node::GetNormalMatrix()
{
	return inverse(transpose(GetTransform()));
}

glm::vec3 Node::GetWorldPosition()
{
	glm::vec4 p4(0,0,0,1);
	glm::vec4 result = GetTransform()*p4;
	return glm::vec3(result/result.w);
}

// eof ///////////////////////////////// class Node