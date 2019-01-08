//----------------------------------------------------//
//                                                    //
// File: ObjLoader.h                                  //
// Obj Loader class. Provides loading of .obj files   //
// Textures should be in TGA format.				  //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//

#ifndef OBJLOADER_H
#define OBJLOADER_H

#pragma once
//using namespace

// includes ////////////////////////////////////////
#include "OBJMaterial.h"

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////
enum resultcode 
{ 
	RESULT_OK, 
	RESULT_FILE_NOT_FOUND, 
	RESULT_BAD_FORMAT, 
	RESULT_OUT_OF_MEMORY,
	RESULT_FORMAT_WARNING
};

struct Triangle
{
	glm::vec3							plane_normal;
	flt									area;
	glm::vec3							tangent[3];
	glm::vec3							bitangent[3];
	glm::vec3							vertex[3];
	glm::vec3							normal[3];
	glm::vec3							texcoord[2][3];

	Triangle():
		plane_normal(glm::vec3(0,1,0)),
		area(1)
	{
		for(int i = 0; i < 3; ++i)
		{
			vertex[i] = glm::vec3(0);
			normal[i] = glm::vec3(0);
			texcoord[0][i] = glm::vec3(0);
			texcoord[1][i] = glm::vec3(0);
		}
	}

	void calcTangentBitangent()
	{
		plane_normal = glm::cross(vertex[1]-vertex[0],vertex[2]-vertex[0]);
		area = glm::length(plane_normal)/flt(2);
		plane_normal = glm::normalize(plane_normal);

		// gepap: implementation of the method found in Math. for CG and Game progr., E. Lengyel.

		flt u21, v21, u31, v31, det; 
		u21 = texcoord[0][1].s - texcoord[0][0].s;
		v21 = texcoord[0][1].t - texcoord[0][0].t;
		u31 = texcoord[0][2].s - texcoord[0][0].s;
		v31 = texcoord[0][2].t - texcoord[0][0].t;
		det = u21 * v31 - u31 * v21;
		glm::vec3 q2 = vertex[1]-vertex[0];
		glm::vec3 q3 = vertex[2]-vertex[0];
		glm::vec3 t;

		if (det!=0.0f)
		{
			t.x = (v31*q2.x-v21*q3.x)/det;
			t.y = (v31*q2.y-v21*q3.y)/det;
			t.z = (v31*q2.z-v21*q3.z)/det;
			t = glm::normalize(t);
		}
		else
		{
			t = glm::vec3(0,0,1);
			if (glm::abs((glm::dot(plane_normal,t)-1)) < flt(1.0e-07f ))
				t = glm::vec3(0,1,0);
			t = glm::cross(t,plane_normal);
		}

		for (short vi = 0; vi < 3; ++vi)
		{
			bitangent[vi] = glm::normalize(glm::cross(normal[vi], t));
			tangent[vi] =   glm::normalize(glm::cross(bitangent[vi], normal[vi]));
		}
	}
};

struct PrimitiveGroup
{
	std::vector<Triangle>				primitives;
	unsigned long						num_primitives;
	OBJMaterial*						material_used;
	int									material_index;

	PrimitiveGroup(unsigned long hint_size) :
		material_used(nullptr), 
		material_index(-1), num_primitives(0)
	{
		primitives.reserve(hint_size);
	}

	~PrimitiveGroup()
	{
		primitives.clear();
	}

	void PrimitiveGroup::addPrimitive(Triangle & prim)
	{
		primitives.push_back(prim);
		num_primitives++;
	}

	void PrimitiveGroup::resize()
	{
		primitives.resize(primitives.size());
	}

	void PrimitiveGroup::clear()
	{
		primitives.clear(); 
		num_primitives = 0; 
	}
};

struct OBJMesh
{
	std::vector<PrimitiveGroup>			elements;
	unsigned long						num_elements;
	unsigned long						num_primitives;
	std::vector<OBJMaterial*>			materials;
	std::string							filename;
	std::string							path;
	bool								loaded;

	OBJMesh():
		num_elements(0),
		num_primitives(0),
		loaded(false)
	{

	}

	int findMaterialByName(std::string& name, OBJMaterial ** mat)
	{
		for (unsigned int i= 0; i< materials.size(); i++ )
		{
			if (materials[i]!=nullptr && materials[i]->m_name==name)
			{
				*mat = materials[i];
				return i;
			}
		}
		*mat = nullptr;
		return -1;
	}

	void addElement(PrimitiveGroup group)
	{
		elements.push_back(group);
		num_elements++;
		num_primitives+=group.num_primitives;
	}
};

class OBJLoader
{
protected:
	// protected variable declarations


	// protected function declarations


private:
	// private variable declarations
	resultcode							result;
	OBJMesh *							current_object;
	glm::vec3*							vertices;
	glm::vec3*							normals;
	glm::vec2*							texcoords;
	virtual void						cleanup(void);
	std::string							mtl_filename;

	// private function declarations
	void								loadMaterials(std::string& mtllib, std::vector<OBJMaterial*>& material_map);
	void								loadGeometry(void);
	void								clearBufferData(void);

public:
	// Constructor
	OBJLoader(void);

	// Destructor
	~OBJLoader(void);

	// public function declarations
	class OGLMesh*						loadMesh(std::string filename, std::string path, bool use_mipmaps);

	// get functions


	// set functions

};

#endif //OBJLOADER_H

// eof ///////////////////////////////// class OBJLoader