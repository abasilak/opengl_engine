//----------------------------------------------------//
//                                                    //
// File: ObjLoader.cpp                                //
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

// includes ////////////////////////////////////////
#include "../HelpLib.h"	   // - Library for including GL libraries, checking for OpenGL errors, writing to Output window, etc.
#include "OBJMaterial.h"   // - Header file for the OBJMaterial class
#include "OBJLoader.h"	   // - Header file for the OBJLoader class
#include "OGLMesh.h"	   // - Header file for the OGLMesh class

#include <iostream>		   // - Header file for IO stream
#include <fstream>		   // - Header file for file stream
#include <sstream>		   // - Header file for string stream

// Constructor
OBJLoader::OBJLoader(void) : 
	current_object(nullptr), 
	mtl_filename(""),
	vertices(nullptr),
	normals(nullptr),
	texcoords(nullptr)
{

}

// Destructor
OBJLoader::~OBJLoader(void)
{

}

// other functions
void OBJLoader::cleanup()
{
	clearBufferData();
	SAFE_DELETE(current_object);
}

void OBJLoader::loadGeometry()
{
	if (result!=RESULT_OK)
		return;
	std::ifstream diskfile;
	std::string fileNameWithPath = current_object->path + "\\" + current_object->filename;
	diskfile.open(fileNameWithPath.c_str(), std::ifstream::in);
	if (diskfile.bad() || !diskfile.is_open())
	{
		result = RESULT_FILE_NOT_FOUND;
		Print("%s not found in path %s", current_object->filename.c_str(), current_object->path.c_str());
		return;
	}

	// buffer the file in memory
	diskfile.seekg (0, std::ios::end);
	unsigned int length = diskfile.tellg();
	diskfile.seekg (0, std::ios::beg);
	char* buffered_content = new char [length];
	diskfile.read (buffered_content,length);
	diskfile.close();

	std::istringstream file = std::istringstream(std::string(buffered_content));

	std::string buf;

	// count attributes
	unsigned long cn=0, ct=0, cv=0;
	unsigned long ni=0, ti=0, vi=0;
	unsigned long max_faces = 0, faces = 0;
	unsigned long total_faces = 0;
	while (file.good()) 
	{
		file >> std::skipws >> buf;
		if (buf[0]=='f') // add face
		{
			faces++;
			total_faces++;
		}
		else if (buf[0]=='v') // add vertex
		{	
			if (buf.size()==1)
				cv++;
			else if (buf[1]=='n') // add normal
				cn++;
			else if (buf[1]=='t') // add texture coordinate pair
				ct++;
		}
		else if ( buf[0]=='g' || (buf[0]=='u' && buf=="usemtl") )
		{
			max_faces = faces>max_faces?faces:max_faces;
			faces=0;
		}
	}

	Print("%s: Total faces: %d, total vertices: %d, total normals: %d, total texcoord pairs: %d", current_object->filename.c_str(), total_faces, cv, cn, ct);
	max_faces = faces>max_faces?faces:max_faces;

	file.clear();
	file.str(buffered_content);
	SAFE_DELETE(buffered_content);


	//file = std::istringstream(std::string(buffered_content));

	PrimitiveGroup group(max_faces);

	if (cv == 0)
	{
		Print("No vertices found for mesh %s.", current_object->filename.c_str());
		result = RESULT_BAD_FORMAT;
		return;
	}
	vertices = new glm::vec3[cv];
	if (cn == 0)
	{
		Print("No normals found for mesh %s.", current_object->filename.c_str());
		result = RESULT_BAD_FORMAT;
		return;
	}
	normals = new glm::vec3[cn];
	if (ct == 0)
	{
		Print("No texcoords found for mesh %s.", current_object->filename.c_str());
	}
	else
		texcoords = new glm::vec2[ct];

	if (current_object->materials.empty())
		current_object->materials.push_back(new OBJMaterial()); // add default material

	OBJMaterial * cur_material = current_object->materials[0]; // track active material
	int cur_material_index = 0;

	unsigned long line=0;

	while (file.good()) 
	{
		buf.clear();
		file >> std::skipws >> buf;
		if (buf=="#")
		{
			char restline[2048];
			file.getline(restline,2048);
		}
		else if (buf=="mtllib")
		{
			// found a new material template library. Append discovered materials
			std::string mtllib;
			file >> std::skipws >> mtllib;
			std::vector<OBJMaterial*> imported_materials;
			loadMaterials(mtllib, imported_materials);
			if (result != RESULT_OK)
			{
				clearBufferData();
				return;
			}
			for (size_t i=0; i<imported_materials.size(); i++ )
			{
				current_object->materials.push_back(imported_materials[i]);
			}
		}
		else if (buf=="usemtl")
		{
			// add currently filled group to the object and create a new one
			if (group.num_primitives>0)
			{
				group.resize();
				current_object->addElement(group);
				group.clear();
				group = PrimitiveGroup(max_faces);
			}
			// locate material and assign to new group
			std::string group_mat_name;
			file >> std::skipws >> group_mat_name;
			cur_material_index = current_object->findMaterialByName(group_mat_name, &cur_material);
			if (cur_material == nullptr)
			{
				cur_material = current_object->materials[0];
				cur_material_index = 0;
			}
			group.material_used = cur_material;
			group.material_index = cur_material_index;
		}
		else if (buf=="g")
		{
			// add currently filled group to the object and create a new one
			if (group.num_primitives>0)
			{
				group.resize();
				current_object->addElement(group);
				group.clear();
				group = PrimitiveGroup(max_faces);
			}
			group.material_used = cur_material;
			group.material_index = cur_material_index;
			char restline[2048];
			file.getline(restline,2048);
		}
		else if (buf=="v") // add vertex
		{
			glm::vec3 v;
			file >> std::skipws >> v.x >> std::skipws >> v.y >> std::skipws >> v.z;
			vertices[vi++] = v;
		}
		else if (buf=="vn") // add normal
		{
			glm::vec3 n;
			file >> std::skipws >> n.x >> std::skipws >> n.y >> std::skipws >> n.z;
			normals[ni++] = n;
		}
		else if (buf=="vt") // add texture coordinate pair
		{
			glm::vec2 t;
			file >> std::skipws >> t.x >> std::skipws >> t.y;
			texcoords[ti++] = t;
		}
		else if (buf=="f") // found a face
		{
			std::string tuple;
			Triangle tr;
			bool undefined_normals = false;
			for (int i=0; i<3; i++) // parse all triangle vertex attribute tuples
			{
				file >> std::skipws >> tuple;

				unsigned long iv=0, it=0, in=0;
				sscanf_s(tuple.c_str(), "%lu/%lu/%lu", &iv, &it, &in);
				if (it==0)
					sscanf_s(tuple.c_str(), "%lu//%lu", &iv, &in);

				if (iv==0 || iv>cv) // undefined vertex index is not allowed: error
				{
					result = RESULT_BAD_FORMAT;
					clearBufferData();
					return;
				}
				tr.vertex[i] = vertices[iv-1];

				if (it==0) // no tex coords, invent some;
				{
					tr.texcoord[0][i] = glm::vec3(0,0,0);
				}
				else if (it>ct) // illegal tex coord index, keep parsing but issue a warning
				{
					result = RESULT_FORMAT_WARNING;
					tr.texcoord[0][i] = glm::vec3(0,0,0);
				}
				else
				{
					tr.texcoord[0][i] = glm::vec3(texcoords[it-1],0); 
				}

				if (in==0) // no normals defined, use triangle plane normal 
				{
					undefined_normals = true;
				}
				else if (in>cn) // illegal normal index, keep parsing but issue a warning
				{
					result = RESULT_FORMAT_WARNING;
					undefined_normals = true;
				}
				else
				{
					tr.normal[i] = normals[in-1];
				}


			} // for all vertices
			tr.calcTangentBitangent(); // compute attributes
			if (undefined_normals)
			{
				tr.normal[0] = tr.normal[1] = tr.normal[2] = tr.plane_normal;
				tr.calcTangentBitangent();
			}
			group.addPrimitive(tr);
		} 
		line++;
	} // while

	if (group.num_primitives>0)
	{
		group.resize();
		group.material_used = cur_material;
		group.material_index = cur_material_index;
		current_object->addElement(group);
		group.clear();
	}

	clearBufferData();
}

void OBJLoader::clearBufferData()
{
	SAFE_DELETE_ARRAY_POINTER(vertices);
	SAFE_DELETE_ARRAY_POINTER(normals);
	SAFE_DELETE_ARRAY_POINTER(texcoords);
}

OGLMesh* OBJLoader::loadMesh(std::string filename, std::string path, bool use_mipmaps)
{
	if (current_object != nullptr)
		cleanup();

	OGLMesh* oglmesh = nullptr;
	result = RESULT_OK;
	current_object = new OBJMesh();
	current_object->filename = filename;
	current_object->path = path;

	loadGeometry();

	current_object->loaded = result == RESULT_OK;

	if (!current_object->loaded)
	{
		Print("Obj Mesh %s loading error %d.", current_object->filename.c_str(), result);
		cleanup();
	}
	else
	{
		Print("Obj Mesh %s successfully loaded. Total elements: %d, total primitives: %d",  current_object->filename.c_str(), current_object->num_elements, current_object->num_primitives);
		oglmesh = new OGLMesh(current_object->filename, current_object->path);
		if (oglmesh->loadToOpenGL(*current_object, use_mipmaps))
		{
			Print("Loaded %s mesh to OpenGL. Total elements: %d, total primitives: %d, total vertices: %d", current_object->filename.c_str(), oglmesh->getNumElements(), oglmesh->getNumPrimitives(), oglmesh->getNumVertices());
		}
		else
		{
			oglmesh->release();
			SAFE_DELETE(oglmesh);
		}
		cleanup();
	}

	return oglmesh;
}

void OBJLoader::loadMaterials(std::string& mtllib, std::vector<OBJMaterial*>& material_map)
{
	if (mtllib.empty())
	{
		result = RESULT_FILE_NOT_FOUND;
		Print("Empty material file provided");
		return;
	}

	std::ifstream file;
	std::string fileNameWithPath = current_object->path + "\\" + mtllib;
	file.open(fileNameWithPath.c_str(), std::ifstream::in);
	if (file.bad() || !file.is_open())
	{
		result = RESULT_FILE_NOT_FOUND;
		Print("%s material not found in path %s", mtllib.c_str(), current_object->path.c_str());
		return;
	}

	std::string buf;
	OBJMaterial default_mat; // setup a dummy material to prevent bad file syntax problems
	OBJMaterial *current_mat = &default_mat;

	while (file.good()) 
	{
		file >> std::skipws >> buf;
		if (buf=="newmtl")
		{
			current_mat = new OBJMaterial();
			file >> std::skipws >> current_mat->m_name;
			material_map.push_back(current_mat);
		}
		else if (buf=="Kd")
		{
			file >> std::skipws >> current_mat->m_diffuse.x >> std::skipws >> current_mat->m_diffuse.y >> std::skipws >> current_mat->m_diffuse.z;
		}
		else if (buf=="Ke")
		{
			file >> std::skipws >> current_mat->m_emission.x >> std::skipws >> current_mat->m_emission.y >> std::skipws >> current_mat->m_emission.z;
		}
		else if (buf=="Ks")
		{
			file >> std::skipws >> current_mat->m_specular.x >> std::skipws >> current_mat->m_specular.y >> std::skipws >> current_mat->m_specular.z;
		}
		else if (buf=="d")
		{
			flt opacity;
			file >> std::skipws >> opacity;
			current_mat->m_opacity = opacity;
		}
		else if (buf=="Ns")
		{
			flt shininess;
			file >> std::skipws >> shininess;
			current_mat->m_gloss = glm::min(shininess/flt(127),flt(1));
		}
		else if (buf=="map_Kd")
		{
			file >> std::skipws >> current_mat->m_diffuse_opacity_tex_file;
		}
		else if (buf=="map_Ks")
		{
			file >> std::skipws >> current_mat->m_specular_gloss_tex_file;
		}
		else if (buf=="map_Ke")
		{
			file >> std::skipws >> current_mat->m_emission_tex_file;
		}
		else if (buf=="map_bump")
		{
			file >> std::skipws >> current_mat->m_normal_tex_file;
		}
	}
	file.close();


	// add a default material, if none was found
	if (material_map.empty())
	{	
		material_map.push_back(new OBJMaterial());		
		material_map[0]->m_name = "default";
	}
}