//----------------------------------------------------//
//                                                    //
// File: TGA.cpp                                      //
// TGA class. Loads TGA files                         //
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
#include "TGA.h"		   // - Header file for the TGA class

// Constructor
TGA::TGA():
m_texture_info(),
m_palette(NULL)
{

}

// Destructor
TGA::~TGA()
{
	SAFE_DELETE(m_palette)
}

// other functions
bool TGA::TGAError(std::string& error_msg, const char* msg, FILE* file)
{
	error_msg += msg;
	if(file != NULL) fclose(file);
	return false;
}

bool TGA::Load(std::string& filename, unsigned char** texture_data, std::string& error_msg)	
{
	FILE * file = NULL;
	
	fopen_s(&file, filename.c_str(), "rb");
	
	if (!file) return TGAError(error_msg, "Could not find file.", file);

	if(fread(&m_identsize, sizeof(m_identsize), 1, file) == 0)
		return TGAError(error_msg, "Could not read TGA header", file);
	if(fread(&m_colourmaptype, sizeof(m_colourmaptype), 1, file) == 0)
		return TGAError(error_msg, "Could not read TGA header", file);
	if(fread(&m_imagetype, sizeof(m_imagetype), 1, file) == 0)
		return TGAError(error_msg, "Could not read TGA header", file);
	if(fread(&m_colourmapstart, sizeof(m_colourmapstart), 1, file) == 0)
		return TGAError(error_msg, "Could not read TGA header", file);
	if(fread(&m_colourmaplength, sizeof(m_colourmaplength), 1, file) == 0)
		return TGAError(error_msg, "Could not read TGA header", file);
	if(fread(&m_colourmapbits, sizeof(m_colourmapbits), 1, file) == 0)
		return TGAError(error_msg, "Could not read TGA header", file);
	if(fread(&m_xstart, sizeof(m_xstart), 1, file) == 0)
		return TGAError(error_msg, "Could not read TGA header", file);
	if(fread(&m_ystart, sizeof(m_ystart), 1, file) == 0)
		return TGAError(error_msg, "Could not read TGA header", file);
	if(fread(&m_width, sizeof(m_width), 1, file) == 0)
		return TGAError(error_msg, "Could not read TGA header", file);
	if(fread(&m_height, sizeof(m_height), 1, file) == 0)
		return TGAError(error_msg, "Could not read TGA header", file);
	if(fread(&m_bits, sizeof(m_bits), 1, file) == 0)
		return TGAError(error_msg, "Could not read TGA header", file);
	if(fread(&m_descriptor, sizeof(m_descriptor), 1, file) == 0)
		return TGAError(error_msg, "Could not read TGA header", file);

	if (m_imagetype != 1  && m_imagetype != 2  && m_imagetype != 3  && m_imagetype != 9 && 
		m_imagetype != 10 && m_imagetype != 11 && m_imagetype != 32 && m_imagetype != 33)
	{
		if (m_imagetype == 0) return TGAError(error_msg, "No image available. TGA image type is 0.", file);
		else return TGAError(error_msg, "Unsupported TGA image type. Supported image types are 1, 2, 3, 9, 10, 11, 32 and 33", file);
	}
	if (m_bits !=8 && m_bits != 16 && m_bits != 24 && m_bits != 32)
		return TGAError(error_msg, "Unsupported TGA pixel depth. Supported pixel depths are 8, 16, 24, and 32 bits", file);
	if (m_colourmaptype != 0 && m_colourmaptype != 1)
		return TGAError(error_msg, "Unsupported TGA colour map type. Supported colour map types are 0 and 1", file);

	m_texture_info.size		= m_width * m_height * (m_bits / 8);

	m_texture_info.dimensions	= 2;
	if (m_height <= 1) m_texture_info.dimensions = 1;

	if (m_texture_info.size < 1)
		return TGAError(error_msg, "Could not read TGA header", file);

	if (m_colourmaptype == 1)
	{
		m_palette = new unsigned char[m_colourmaptype * m_colourmaplength];
		if(fread(m_palette, 1, m_colourmaptype * m_colourmaplength, file) != (size_t)(m_colourmaptype * m_colourmaplength))
			return TGAError(error_msg, "Could not read TGA colour palette", file);
	}

	(*texture_data) = new unsigned char[m_texture_info.size];
	
	if((*texture_data) == NULL)
		return TGAError(error_msg, "Could not allocate memory for TGA image", file);

	if (m_imagetype == 1)
	{
		// Indexed
		if(fread((*texture_data), 1, m_texture_info.size, file) != m_texture_info.size)
			return TGAError(error_msg, "Could not read TGA image data", file);

		for (unsigned char i = 0; i < m_texture_info.size; ++i)
			(*texture_data)[i] = m_palette[(*texture_data)[i]];
	}
	else if (m_imagetype == 2)
	{
		// RGB
		if(fread((*texture_data), 1, m_texture_info.size, file) != m_texture_info.size)
			return TGAError(error_msg, "Could not read TGA image data", file);
	}
	else if (m_imagetype == 3)
	{
		// Grey
		if(fread((*texture_data), 1, m_texture_info.size, file) != m_texture_info.size)
			return TGAError(error_msg, "Could not read TGA image data", file);
	}
	else if (m_imagetype  > 8)
	{

	}
	
	fclose(file);

	return true;
}