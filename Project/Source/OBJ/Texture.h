//----------------------------------------------------//
//                                                    //
// File: Texture.h                                    //
// Texture class. Limited support for TGA files	      //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//
#ifndef TEXTURE_H
#define TEXTURE_H

#pragma once
//using namespace

// includes ////////////////////////////////////////
#include "TGA.h"

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class Texture
{
protected:
	// protected variable declarations


	// protected function declarations


private:
	// private variable declarations
	unsigned int						m_gl_texture_id;
	int									m_data_type;
	unsigned int						m_internal_format;
	unsigned char*						m_data;
	TGA*								m_tga;
	std::string							m_filename;
	unsigned int						m_width;
	unsigned int						m_height;
	unsigned int						m_dimensions;
	unsigned int						m_bits;
	unsigned int						m_size;
	int									m_format;
	bool								m_build_mipmaps;
	bool								m_loaded;
	std::string							m_error_msg;

public:
	// Constructor
	Texture(const std::string& filename, const bool build_mipmaps = false);

	// Destructor
	~Texture(void);

	// public function declarations

	void								LoadTGA(void);
	bool								read_error(char* text = "", FILE* File = NULL);
	void								print_dimensions(void);
	void								create(void);
	void								destroy(void);
	void								GenerateTexture(void);
	void								BindTexture(void) const;
	void								UnbindTexture(void) const;

	// get functions
	const int							get_texture_gl_id(void) const				{return m_gl_texture_id;}
	const unsigned char*				get_data(void) const						{return m_data;}
	const std::string&					get_filename(void) const					{return m_filename;}
	const unsigned int					get_dimensions(void) const					{return m_dimensions;}
	const unsigned int					get_width(void) const						{return m_width;}
	const unsigned int					get_height(void) const						{return m_height;}
	const unsigned int					get_bits(void) const						{return m_bits;}
	const unsigned int					get_size(void) const						{return m_size;}
	bool								loaded(void) const							{return m_loaded;}

	// set functions
};

#endif //TEXTURE_H

// eof ///////////////////////////////// class Texture