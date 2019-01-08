//----------------------------------------------------//
//                                                    //
// File: TransformNode.h							  //
// This scene graph is a basic example for the		  //
// object relational management of the scene          //
// The transform node is a group node containing a    //
// series of affine transformations applied directly  //
// its children nodes								  //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//
#ifndef TRANSFORMNODE_H
#define TRANSFORMNODE_H

#pragma once
//using namespace

// includes ////////////////////////////////////////
#include "GroupNode.h"

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class TransformNode : public GroupNode
{
protected:
	// protected variable declarations
	float								m_angle;
	glm::vec3							m_axis;
	glm::vec3							m_scale;
	glm::vec3							m_translation;
	glm::mat4x4							m_matrix;

	// protected function declarations
	virtual void						CalcMatrix(void);


private:
	// private variable declarations


	// private function declarations


public:
	// Constructor
	TransformNode(const char* name);

	// Destructor
	~TransformNode(void);

	// public function declarations
	virtual void						Init(void);
	virtual void						Update(void);
	virtual void						Draw(int shader_type);

	// get functions
	virtual glm::mat4x4					GetTransform(void);

	virtual glm::vec3&					GetTranslation(void)						{ return m_translation; }
	virtual float						GetRotationAngle(void)						{ return m_angle; }
	virtual glm::vec3&					GetRotationAxis(void)						{ return m_axis; }
	virtual glm::vec3&					GetScale(void)								{ return m_scale; }

	// set functions
	void								SetRotation(float theta, float ax, float ay, float az);
	void								SetTranslation(float ox, float oy, float oz);
	void								SetScale(float sx, float sy, float sz);
	void								SetMatrix(glm::mat4x4 m);
};

#endif //TRANSFORMNODE3D_H

// eof ///////////////////////////////// class TransformNode