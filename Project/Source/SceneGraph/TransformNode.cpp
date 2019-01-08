//----------------------------------------------------//
//                                                    //
// File: TransformNode.cpp							  //
// This scene graph is a basic example for the		  //
// object relational management of the scene          //
// The transform node is a group node containing a    //
// series of affine transformations applied directly  //
// to its children nodes							  //
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
#include "TransformNode.h" // - Header file for the TransformNode class


// defines /////////////////////////////////////////


// Constructor
TransformNode::TransformNode(const char* name):
GroupNode(name),
m_axis(1,0,0),
m_scale(1,1,1),
m_angle(0),
m_translation(0,0,0),
m_matrix(glm::mat4x4(1))
{
	CalcMatrix();
}

// Destructor
TransformNode::~TransformNode()
{

}

// other functions
void TransformNode::Update()
{
	GroupNode::Update();
}

void TransformNode::Draw(int shader_type)
{
	for (unsigned int i=0; i<children.size();i++)
		children.at(i)->Draw(shader_type);
}

void TransformNode::CalcMatrix()
{
	glm::mat4x4 S, T, R;
	S = glm::scale(m_scale);
	R = glm::rotate(m_angle, m_axis);
	T = glm::translate(m_translation);
	m_matrix = T*R*S;
}

glm::mat4x4 TransformNode::GetTransform()
{
	if (m_parent!=nullptr)
		return m_parent->GetTransform() * m_matrix;
	else
		return m_matrix;
}

void TransformNode::SetRotation(float theta, float ax, float ay, float az)
{
	m_angle = theta;
	m_axis = glm::vec3(ax,ay,az);
	CalcMatrix();
}

void TransformNode::SetTranslation(float ox, float oy, float oz)
{
	m_translation = glm::vec3(ox,oy,oz);
	CalcMatrix();
}

void TransformNode::SetScale(float sx, float sy, float sz)
{
	m_scale = glm::vec3(sx,sy,sz);
	CalcMatrix();
}

void TransformNode::SetMatrix(glm::mat4x4 m)
{
	m_matrix = m;
}

void TransformNode::Init()
{
	GroupNode::Init();
}

// eof ///////////////////////////////// class TransformNode