#ifndef TEXTRENDERING_H
#define TEXTRENDERING_H

#pragma once

#include "HelpLib.h"
#include "Main.h"
#include "Shaders.h"

#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character
{
	GLuint TextureID;   // ID handle of the glyph texture
	ivec2  Size;		// Size of glyph
	ivec2  Bearing;		// Offset from baseline to left/top of glyph
	GLuint Advance;		// Horizontal offset to advance to next glyph
};

class TextRendering
{
private:
	// private variable declarations
	GLuint					m_vao, m_vbo;
	map<GLchar, Character>	m_characters;

	FT_UInt					m_character_size;
	Shader				   *m_shader_text_rendering;

	const vec3				m_color;
	const string			m_font;

public:
	GLboolean				m_enabled;
	vector <string>		   *m_output;

	// Constructor
	TextRendering() : m_enabled(true), m_font("times.ttf"), m_character_size(20), m_color(0.0f, 0.0f, 1.0f) {
		m_output = new vector <string>();
	}
	
	// Destructor
	~TextRendering(void)
	{
		SAFE_DELETE(m_shader_text_rendering);

		// Iterate through all characters
		for (GLuint i = 0; i < 128; i++)
			glDeleteTextures(1, &m_characters[i].TextureID);
	};

	void InitFT(void);
	void InitVAO(void);
	bool CreateShader(void);
	void Draw(GLfloat width, GLfloat height);
	void Draw(string text, GLfloat x, GLfloat y, GLfloat scale, GLfloat width, GLfloat height);
};

#endif //TEXT_RENDERING_H

// eof ///////////////////////////////// class Text