#ifndef SCREEN_QUAD_H
#define SCREEN_QUAD_H

#pragma once

#include "Light.h"
#include "Shaders.h"

class ScreenQuad
{
	GLuint			m_vao;
	GLuint			m_window_percentage;
	ViewportGL*		m_viewport;
	
	GLuint					m_id;
	
	vector<Shader*>			m_shaders;
	vector<vector<GLuint>>	m_textures_ids;
	vector<vector<string>>  m_textures_strings;

public:

	ScreenQuad(GLuint window_percentage) : m_id(0u), m_window_percentage(window_percentage)
	{
		InitVAO();
	}

	~ScreenQuad(void) 
	{
		SAFE_DELETE (m_viewport);
	};

	void	Draw				(void);
	void	SetViewport			(GLuint width, GLuint height);
	void	InitVAO				(void);

	void	Next				(void)					{ if (m_id < m_shaders.size() - 1) m_id++; }
	void	Prev				(void)					{ if (m_id > 0					 ) m_id--; }
	string  GetTextureString	(void)					{ return m_textures_strings[m_id][0]; }
	GLfloat GetPercentage		(void)					{ return m_window_percentage; }

	void	AddTextureList(vector<GLuint> tex_ids, vector<string> tex_strings)
	{ 
		m_textures_ids.push_back(tex_ids);
		m_textures_strings.push_back(tex_strings);
	}
	void	AddShader			(Shader*		shader  ) { m_shaders.push_back(shader); }

	void	IncreasePercentage	(GLuint width, GLuint height);
	void	DecreasePercentage	(GLuint width, GLuint height);
};

#endif //SCREEN_QUAD_H

// eof ///////////////////////////////// class SCREEN_QUAD