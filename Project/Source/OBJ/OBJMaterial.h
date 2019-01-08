//----------------------------------------------------//
//                                                    //
// File: OGLMaterial.h                                //
// OGLMaterial holds the reflectance properties for   //
// the loaded models								  //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//
#ifndef OBJMATERIAL_H
#define OBJMATERIAL_H

#pragma once
//using namespace

// includes ////////////////////////////////////////


// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

enum MATERIAL_MAP_TYPE
{
	MPT_MATERIAL_MAP_DIFFUSE_OP			= 0,			// map_Kd   (RGBA)
	MPT_MATERIAL_MAP_NORMAL				= 1,			// map_bump (greyscale->height)
	MPT_MATERIAL_MAP_SPECULAR_GLOSS		= 2,			// map_Ks   (Ks_R, Ks_G, Ks_B, Kexp)
	MPT_MATERIAL_MAP_EMISSION			= 3,			// map_Ke   (greyscale->intensity)
	MPT_MATERIAL_MAP_COUNT				= 4
};
/*
0x01 : 00000001
0x02 : 00000010
0x04 : 00000100
0x08 : 00001000	
0x10 : 00010000
0x20 : 00100000
0x40 : 01000000
0x80 : 10000000
*/
enum MATERIAL_MAP_FLAGS
{
	MPF_DIFFUSE_OP				= 1 << 0,
	MPF_NORMAL					= 1 << 1,
	MPF_SPECULAR_GLOSS			= 1 << 2,
	MPF_EMISSION				= 1 << 3
};

class OBJMaterial
{
protected:
	// protected variable declarations


	// protected function declarations


private:
	// private variable declarations


	// private function declarations


public:
	// Constructor
	OBJMaterial(void);

	// Destructor
	~OBJMaterial(void);

	// public variable declarations
	glm::vec3							m_diffuse;
	glm::vec3							m_specular;
	glm::vec3							m_emission;
	flt									m_gloss;
	flt									m_opacity;
	std::string							m_diffuse_opacity_tex_file;
	std::string							m_specular_gloss_tex_file;
	std::string							m_emission_tex_file;
	std::string							m_normal_tex_file;
	bool								m_diffuse_opacity_tex_loaded;
	bool								m_specular_gloss_tex_loaded;
	bool								m_emission_tex_loaded;
	bool								m_normal_tex_loaded;							
	class Texture*						m_diffuse_opacity_tex;
	class Texture*						m_specular_gloss_tex;
	class Texture*						m_emission_tex;
	class Texture*						m_normal_tex;
	std::string							m_name;
		

	// public function declarations
	bool								operator == (OBJMaterial& m);

	void								dump();

	// get functions


	// set functions


};

#endif //OBJMATERIAL_H

// eof ///////////////////////////////// class OBJMaterial