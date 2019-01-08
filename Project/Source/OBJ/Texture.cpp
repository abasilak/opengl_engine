//----------------------------------------------------//
//                                                    //
// File: Texture.cpp                                  //
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

// includes ////////////////////////////////////////
#include "../HelpLib.h"	   // - Library for including GL libraries, checking for OpenGL errors, writing to Output window, etc.
#include "Texture.h"	   // - Header file for the Texture class
#include "TGA.h"		   // - Header file for the TGA class
  
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1

// Constructor
Texture::Texture(const std::string& filename, const bool build_mipmaps):
m_data_type(GL_UNSIGNED_BYTE),
m_internal_format(GL_RGBA),
m_data(NULL),
m_tga(NULL),
m_filename(filename),
m_width(1),
m_height(1),
m_dimensions(0),
m_bits(1),
m_size(0),
m_format(-1),
m_build_mipmaps(build_mipmaps),
m_loaded(false),
m_error_msg("")
{

	LoadTGA();

	if (!m_loaded)
		Print("Could not load texture: %s Error: %s", m_filename.c_str(), m_error_msg.c_str());
}


// Destructor
Texture::~Texture()
{

}

// other functions
void Texture::create()
{

}

void Texture::destroy()
{
	SAFE_DELETE(m_data)

	glDeleteTextures(1, &m_gl_texture_id);

	Print("Disposed texture with id: %d", m_gl_texture_id);

	SAFE_DELETE(m_tga)
	m_bits = 0;
	m_width = 1;
	m_height = 1;
	m_loaded = false;
	m_error_msg = "";
}

void Texture::BindTexture(void) const
{
	glBindTexture(GL_TEXTURE_2D, m_gl_texture_id);
}	

void Texture::UnbindTexture(void) const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::GenerateTexture()
{
	// to create a texture we use the following:
	// Generate the OpenGL texture id
	glGenTextures(1, &m_gl_texture_id);

	// Bind this texture to its id
	glBindTexture(GL_TEXTURE_2D, m_gl_texture_id);

	// set some parameters
	// When you bind a texture, you can specify how the texture will be applied to the object
	// glTexParameter* is used for this
	// for example glTexParameteri(GLenum target, GLenum pname, GLint param)
	// - target specifies the target texture.  It must be either GL_TEXTURE_1D, GL_TEXTURE_2D.
	// - pname specifies the name of a texture parameter. pname can be one of the following: GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T.
	// Other names can be used, look at the documentation http://www.opengl.org/sdk/docs/man/xhtml/glTexParameter.xml
	// - param depends on the pname and can be:

	// for GL_TEXTURE_WRAP_S and GL_TEXTURE_WRAP_T we can use GL_REPEAT and GL_CLAMP. 
	// GL_CLAMP_TO_BORDER causes coordinates that exceed the range [0, 1) to use a default border color.
	// GL_CLAMP_TO_EDGE causes coordinates that exceed the range [0, 1) to use the color at the edge of the texture.
	// GL_REPEAT causes the integer part of the s coordinate to be ignored; the GL uses only the fractional part, thereby creating a repeating pattern.
	// Default is GL_REPEAT.
	//Example:
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// for magnification and minification OpenGL has two parameters that can be set:
	// GL_TEXTURE_MIN_FILTER
	// GL_TEXTURE_MAG_FILTER
	// the GL_TEXTURE_MIN_FILTER is the texture minifying function and is used whenever one texel covers less than one pixel. For example a 32x32 texture showing in 128x128 area.
	// param can be: 
	// GL_NEAREST for selecting the closest pixel, 
	// GL_LINEAR for bilinear interpolation.
	// if mimmaps are enabled (using gluBuild2DMipmaps), then param can be:
	// GL_NEAREST_MIPMAP_NEAREST for nearest mipmap selection and nearest filtering for selecting the texture value to the closest pixel
	// GL_LINEAR_MIPMAP_NEAREST for nearest mipmap selection and linear filtering for selecting the texture value based on bilinear interpolation
	// GL_NEAREST_MIPMAP_LINEAR for linear filtering for mipmap selection and nearest filtering for selecting the texture value to the closest pixel
	// GL_LINEAR_MIPMAP_LINEAR for linear filtering for mipmap selection and linear filtering for selecting the texture value to the closest pixel
	// Default is GL_NEAREST_MIPMAP_LINEAR.
	//Example:
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	// the GL_TEXTURE_MAG_FILTER is the texture magnifying function and is used whenever one texel covers more than one pixel. For example a 32x32 texture showing in a 8x8 area.
	// param can be: 
	// GL_NEAREST for selecting the texture value to the closest pixel, 
	// GL_LINEAR for bilinear interpolation. 
	// Default is GL_LINEAR.
	//Example:
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// glTexImage2D passes the texture to OpenGL
	// glTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
	// target is for the type of texture we have. GL_TEXTURE_2D
	// level is the mipmap level. We use 0.
	// internal format specifies the internal format storage. GL_RGB or GL_RGBA is common here.
	// width is the width of the texture
	// height is the height of the texture
	// border specifies the width of the border if we use GL_TEXTURE_BORDER in glTexParameter. We do now use this, so 0.
	// format is the format of the texture as read from the TGA file. Usually GL_RGB, GL_BGRA, GL_RGBA and GL_BGRA are used
	// type is the data type of the pixel data. GL_UNSIGNED_BYTE is commonly used.
	// pixels points to the texture data as read from the TGA file.
	// NOTE: There are many configurations for glTexImage2D. Consult the documentation for more information.
	glTexImage2D(GL_TEXTURE_2D, 0, m_internal_format, m_width, m_height, 0, m_format, m_data_type, m_data);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Use mipmapping for the texture minification filter
	if (m_build_mipmaps)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

		// glGenerateMipmap builds a series of prefiltered 2D texture maps of decreasing resolution.
		// this helps the texture to look more smooth as we zoom out
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		// no mipmapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	}			

	SAFE_DELETE(m_data)

	Print("Generated texture %s with id: %d", m_filename.c_str(), m_gl_texture_id);
	Print("Dimensions: width: %d, height: %d, size: %d KB", m_width, m_height, m_size);
}

bool Texture::read_error(char* text, FILE* File)
{
	m_error_msg += text;
	if(File != NULL) fclose(File);
	return false;
}

void Texture::LoadTGA(void)
{
	m_tga = new TGA();

	m_error_msg = "";

	unsigned char** data = NULL;

	data = &m_data;

	m_loaded = m_tga->Load(m_filename, data, m_error_msg);

	if (m_loaded == false) return;

	if (m_tga->m_bits == 8)
	{
		m_format = GL_RED;
		m_internal_format = GL_RED;
	}
	else if (m_tga->m_bits == 16 || m_tga->m_bits == 24)
	{
		m_format = GL_BGR;
		m_internal_format = GL_RGB;
	}
	else if (m_tga->m_bits == 32)
	{
		m_format = GL_BGRA;
		m_internal_format = GL_RGBA;
	}

	m_width			= m_tga->m_width;
	m_height		= m_tga->m_height;
	m_bits			= m_tga->m_bits;

	unsigned int texture_size=  m_width * m_height * (m_bits / 8);
	texture_size /= 1024.0f;

	m_size = texture_size;

	m_dimensions	= 2;
	if (m_height <= 1) m_dimensions = 1;
}