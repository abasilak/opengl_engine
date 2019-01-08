//----------------------------------------------------//
//                                                    //
// File: OGLMaterial.cpp                              //
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

// includes ////////////////////////////////////////
#include "../HelpLib.h"		// - Library for including GL libraries, checking for OpenGL errors, writing to Output window, etc.
#include "OBJMaterial.h"	// - Header file for the OBJMaterial class
#include "Texture.h"		// - Header file for the Texture class

// Constructor
OBJMaterial::OBJMaterial(void):
m_diffuse(1), 
m_specular(0), 
m_emission(0),
m_opacity(1),
m_gloss(0), 
m_diffuse_opacity_tex_file(),
m_specular_gloss_tex_file(),
m_emission_tex_file(), 
m_normal_tex_file(), 
m_diffuse_opacity_tex_loaded(false),
m_specular_gloss_tex_loaded(false),
m_emission_tex_loaded(false),
m_normal_tex_loaded(false),
m_diffuse_opacity_tex(nullptr),
m_specular_gloss_tex(nullptr),
m_emission_tex(nullptr),
m_normal_tex(nullptr),
m_name()
{

}

// Destructor
OBJMaterial::~OBJMaterial()
{

}

// other functions
bool OBJMaterial::operator == (OBJMaterial& m)
{
	return (
		m_diffuse						==		m.m_diffuse						&& 
		m_specular						==		m.m_specular					&& 
		m_emission						==		m.m_emission					&& 
		m_gloss							==		m.m_gloss						&& 
		m_opacity						==		m.m_opacity						&& 
		m_diffuse_opacity_tex_file		==		m.m_diffuse_opacity_tex_file	&& 
		m_specular_gloss_tex_file		==		m.m_specular_gloss_tex_file		&& 
		m_emission_tex_file				==		m.m_emission_tex_file			&& 
		m_normal_tex_file				==		m.m_normal_tex_file				&&
		m_diffuse_opacity_tex_loaded	==		m.m_diffuse_opacity_tex_loaded	&& 
		m_specular_gloss_tex_loaded		==		m.m_specular_gloss_tex_loaded	&& 
		m_emission_tex_loaded			==		m.m_emission_tex_loaded			&& 
		m_normal_tex_loaded				==		m.m_normal_tex_loaded			&&
		m_diffuse_opacity_tex			==		m.m_diffuse_opacity_tex			&& 
		m_specular_gloss_tex			==		m.m_specular_gloss_tex			&& 
		m_emission_tex					==		m.m_emission_tex				&& 
		m_normal_tex					==		m.m_normal_tex					&&
		m_name							==		m.m_name
		);
}

void OBJMaterial::dump()
{
	Print("\tMaterial %s info:", m_name.c_str());
	Print("\t\tdiffuse  : % f % f % f", m_diffuse [0], m_diffuse [1], m_diffuse [2]);
	Print("\t\tspecular: % f % f % f", m_specular [0], m_specular [1], m_specular [2]);
	Print("\t\temission : % f % f % f", m_emission[0], m_emission[1], m_emission[2]);
	Print("\t\tgloss: %f \topacity : % f", m_gloss, m_opacity);
		
	if (!m_diffuse_opacity_tex_file.empty())
		Print("\t\tdiffuse_opacity_tex_file: %s\n", m_diffuse_opacity_tex_file.c_str());
	if (!m_specular_gloss_tex_file.empty())
		Print("\t\tspecular_gloss_tex_file: %s\n", m_specular_gloss_tex_file.c_str());
	if (!m_normal_tex_file.empty())
		Print("\t\tnormal_tex_file: %s\n", m_normal_tex_file.c_str());
	if (!m_emission_tex_file.empty())
		Print("\t\temission_tex_file: %s\n", m_emission_tex_file.c_str());
}

// eof ///////////////////////////////// class material