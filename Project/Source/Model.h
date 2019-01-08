//----------------------------------------------------//
//                                                    //
// File: Model.h                                      //
// Omni and directional lights are defined here	      //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//

#ifndef MODEL_H
#define MODEL_H

#pragma once

#include "OBJ/OGLMesh.h"
#include "OBJ/OBJLoader.h"	// - Header file for the OBJ Loader
#include "SceneGraph/TransformNode.h"
#include "SceneGraph/GeometryNode.h"

class Model
{
public:
	string			m_name;

	TransformNode*	m_world_transform;
	GeometryNode*	m_geometry;

	OGLMesh*		m_mesh;

	// Constructor
	Model(string name):	m_name(name) { };

	// Destructor
	~Model(void) {};
};

#endif //MODEL_H

// eof ///////////////////////////////// class Light