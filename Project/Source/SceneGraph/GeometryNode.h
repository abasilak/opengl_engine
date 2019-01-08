//----------------------------------------------------//
//                                                    //
// File: GeometryNode.h								  //
// This scene graph is a basic example for the		  //
// object relational management of the scene          //
// This holds the geometry node which is used for     //
// rendering geometric data							  //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//
#ifndef GEOMETRYNODE_H
#define GEOMETRYNODE_H

#pragma once
//using namespace

// includes ////////////////////////////////////////
#include "Node.h"

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////

// class declarations //////////////////////////////

class GeometryNode: public Node
{
protected:
	// protected variable declarations
	class OGLMesh*						m_ogl_mesh;
		
	// protected function declarations

private:
	// private variable declarations


	// private function declarations
	void								DrawUsingNolighting();
	void								DrawUsingSpotLight();
	void								DrawToShadowMap();
	void								DrawToGBuffer();


public:
	// Constructor
	GeometryNode(const char* name, class OGLMesh* ogl_mesh);

	// Destructor
	~GeometryNode(void);

	// public function declarations
	void								Init(void);
	void								Update(void);
	void								Draw(int shader_type);

	// get functions


	// set functions
	
};

#endif //GEOMETRYNODE_H

// eof ///////////////////////////////// class GeometryNode