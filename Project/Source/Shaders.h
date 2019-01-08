#ifndef SHADERS_H
#define SHADERS_H

#pragma once

#include "ShaderGLSL.h"

// Shaders
// abstract shader class
class Shader
{
public:
	GLint		m_program_id;
	ShaderGLSL*	m_shader;

	Shader(const char* name, bool has_geometry_shader = false)
	{
		m_shader = new ShaderGLSL(name, has_geometry_shader);
	};

	~Shader()
	{
		SAFE_DELETE(m_shader);
	};

	bool LoadAndCompile(void)
	{
		return m_shader->LoadAndCompile();
	}

	virtual void Init	  (void)									{ ; }
	virtual void InitDepth(GLfloat near_depth, GLfloat far_depth)	{ ; }
};

// basic geometry shader
class GeometryShader : public Shader
{
public:
	GLint m_uniform_mvp;

	GeometryShader(const char* name, bool has_geometry_shader = false) : Shader(name, has_geometry_shader) {};

	~GeometryShader(){};

	void Init(void)
	{
		m_program_id = m_shader->GetProgram();
		m_uniform_mvp = glGetUniformLocation(m_program_id, "uniform_mvp");
	}
};

// basic texture color shader
class TextureColorShader : public Shader
{
public:
	TextureColorShader(const char* name, bool has_geometry_shader = false) : Shader(name, has_geometry_shader) {};

	~TextureColorShader(){};

	void Init()
	{
		m_program_id = m_shader->GetProgram();
		GLint uniform_texture_color = glGetUniformLocation(m_program_id, "uniform_texture_color");
		glUseProgram(m_program_id);
			glUniform1i(uniform_texture_color, 0);
		glUseProgram(0);
	}
};

// basic texture depth shader
class TextureDepthShader : public Shader
{
public:
	TextureDepthShader(const char* name, bool has_geometry_shader = false) : Shader(name, has_geometry_shader) {};

	~TextureDepthShader(){};

	void InitDepth(GLfloat near_depth, GLfloat far_depth)
	{
		m_program_id = m_shader->GetProgram();
		GLint uniform_texture_depth = glGetUniformLocation(m_program_id, "uniform_texture_depth");
		GLint uniform_near_depth = glGetUniformLocation(m_program_id, "uniform_z_near");
		GLint uniform_far_depth = glGetUniformLocation(m_program_id, "uniform_z_far");
		glUseProgram(m_program_id);
			glUniform1i(uniform_texture_depth, 0);
			glUniform1f(uniform_near_depth, near_depth);
			glUniform1f(uniform_far_depth , far_depth);
		glUseProgram(0);
	}
};

// spot light shader
class SpotLightShader : public Shader
{
public:
	//Camera
	GLint	m_uniform_camera_position_wcs;

	SpotLightShader(const char* name, bool has_geometry_shader = false) : Shader(name, has_geometry_shader) {};

	~SpotLightShader()
	{
	};

	void Init(void)
	{
		m_program_id = m_shader->GetProgram();

		// Rest uniforms
		// camera
		m_uniform_camera_position_wcs = glGetUniformLocation(m_program_id, "uniform_camera.position_wcs");

		// textures
		glUseProgram(m_program_id);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.diffuse"), 0);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.normal"), 1);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.specular"), 2);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.emission"), 3);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.shadow_map"), 4);

			glUniformBlockBinding(m_program_id, glGetUniformBlockIndex(m_program_id, "Matrices"), 0);
			glUniformBlockBinding(m_program_id, glGetUniformBlockIndex(m_program_id, "Material"), 1);
			glUniformBlockBinding(m_program_id, glGetUniformBlockIndex(m_program_id, "Light"), 2);
		glUseProgram(0);
	}
};

// spot light shader
class DeferredGBufferShader : public Shader
{
public:
	// these uniforms will be the samplers
	DeferredGBufferShader(const char* name, bool has_geometry_shader = false) : Shader(name, has_geometry_shader) {};

	~DeferredGBufferShader() {	};

	void Init(void)
	{
		m_program_id = m_shader->GetProgram();
		
		glUseProgram(m_program_id);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.diffuse"), 0);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.normal"), 1);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.specular"), 2);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.emission"), 3);

			glUniformBlockBinding(m_program_id, glGetUniformBlockIndex(m_program_id, "Matrices"), 0);
			glUniformBlockBinding(m_program_id, glGetUniformBlockIndex(m_program_id, "Material"), 1);
		glUseProgram(0);
	}
};

// basic texture color shader
class DeferredLightingShader : public Shader
{
public:
	DeferredLightingShader(const char* name, bool has_geometry_shader = false) : Shader(name, has_geometry_shader) {};

	~DeferredLightingShader(){};

	void Init()
	{
		m_program_id = m_shader->GetProgram();

		glUseProgram(m_program_id);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.position_wcs"), 0);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.position_lcs"), 1);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.normal"), 2);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.albedo"), 3);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.specular"), 4);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.emission"), 5);
			glUniform1i(glGetUniformLocation(m_program_id, "uniform_textures.shadow_map"), 6);

			glUniformBlockBinding(m_program_id, glGetUniformBlockIndex(m_program_id, "Light"), 2);
		glUseProgram(0);
	}
};

// shadow map shader
class ShadowMapShader : public Shader
{
public:
	GLint m_uniform_mvp;

	ShadowMapShader(const char* name, bool has_geometry_shader = false) : Shader(name, has_geometry_shader) {};

	~ShadowMapShader(){};

	void Init(void)
	{
		m_program_id = m_shader->GetProgram();
		m_uniform_mvp = glGetUniformLocation(m_program_id, "uniform_mvp");
	}
};

#endif //SHADERS_H

// eof ///////////////////////////////// class Shaders