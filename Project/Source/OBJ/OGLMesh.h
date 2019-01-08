//----------------------------------------------------//
//                                                    //
// File: OGLMesh.h                                    //
// OGLMesh holds the OpenGL 3.3 compatible vertex	  //
// data structure and texture information             //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//
#ifndef OGLMESH_H
#define OGLMESH_H

#pragma once
//using namespace

// includes ////////////////////////////////////////
#include "OBJMaterial.h"   // - Header file for the OBJMaterial class
#include "OBJLoader.h"	   // - Header file for the OBJLoader class
#include "OGLMesh.h"	   // - Header file for the OGLMesh class

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

struct VertexData
{
	GLfloat position[3];     // offset:  0  size: 12
	GLfloat normal[3];       // offset: 12  size: 12
	GLfloat texcoord0[2];    // offset: 24  size:  8
	GLfloat texcoord1[2];    // offset: 32  size:  8
	GLfloat tangent[3];      // offset: 40  size: 12
	GLbyte  padding[12];     // offset: 52  size:  12
};							 // Total: 64 bytes/vertex (multiple of 32 bytes)

struct ElementGroup
{
	GLuint start_index;
	GLuint end_index;
	unsigned int material_index;
	unsigned int triangles;
};

class OGLMesh
{
public:
	// protected variable declarations
	bool									released;
	bool									updated;
	std::string								m_fileName;
	std::string								m_path;
	GLuint									vbo;
	GLuint									ibo;
	GLuint									vao;
	ElementGroup *							elements;
	VertexData *							vertexdata;
	GLuint *								indexdata;
	GLint									num_elements;
	GLint									num_vertexdata;
	std::vector<OBJMaterial*>				materials;
	bool									is_dynamic;
	unsigned int							num_total_vertices;
	unsigned int							num_total_primitives;
	unsigned int							num_total_elements;


	// protected function declarations


private:
	// private variable declarations


	// private function declarations


public:
	// Constructor
	OGLMesh(std::string& filename, std::string& path);

	// Destructor
	~OGLMesh(void);

	// public function declarations
	virtual bool							loadToOpenGL(OBJMesh& _mesh, bool use_mipmaps);
	virtual bool							loadTexturesToOpenGL(OBJMesh& _mesh, bool use_mipmaps);
	virtual void							display();   
	virtual void							release();
	virtual void							init();
	virtual void							dump();

	// get functions
	virtual unsigned long					getNumPrimitives() const						{return num_total_primitives;}
	virtual unsigned long					getNumVertices() const							{return num_total_vertices;}
	virtual unsigned long					getNumElements() const							{return num_total_elements;}
	std::string&							getFileName(void)								{return m_fileName;}

	// set functions


};

#endif //OGLMESH_H

// eof ///////////////////////////////// class OGLMesh