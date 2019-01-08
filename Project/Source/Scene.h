#ifndef SCENE_H
#define SCENE_H

#pragma once

#include "HelpLib.h"		// - Library for including GL libraries, checking for OpenGL errors, writing to Output window, etc.
#include "Model.h"			// - Header file for Models
#include "Light.h"			// - Header file for Lights
#include "ScreenQuad.h"		// - Header file for Texture
#include "SceneGraph/Root.h"

enum Shading { Forward, Deferred };

class Scene
{

public:
	// public variable declarations
	GLuint		m_forward_fbo;
	GLuint		m_forward_texture_depth;
	GLuint		m_forward_texture_color;

	GLuint		m_deferred_fbo;
	GLuint		m_deferred_texture_depth;
	GLuint		m_deferred_texture_albedo;
	GLuint		m_deferred_texture_specular;
	GLuint		m_deferred_texture_emission;
	GLuint		m_deferred_texture_position_wcs;
	GLuint		m_deferred_texture_position_lcs;
	GLuint		m_deferred_texture_normal;
	ScreenQuad*	m_deferred_light_shading;

	Shading		m_shading_type;

	Model*		m_model;
	Camera*		m_camera;

	SpotLight*	m_light;

	Root*		m_root_models;
	Root*		m_root_lights;
	
	GLuint		m_ubo_matrices;
	GLuint		m_ubo_material;

	Shader*		m_shader_basic_geometry;
	Shader*		m_shader_texture_depth;
	Shader*		m_shader_texture_color;
	Shader*		m_shader_texture_normal;

	Shader*		m_shader_spotlight;
	Shader*		m_shader_shadow_map;
	Shader*		m_shader_deferred_gbuffer;
	Shader*		m_shader_deferred_lighting;

	// public function declarations

	// Constructor
	Scene() : m_shading_type(Shading::Forward)
	{
		m_camera		= new Camera();
		m_light			= new SpotLight("spotlight");
		m_root_models	= new Root();
		m_root_lights	= new Root();
		m_deferred_light_shading = new ScreenQuad(1);
	};

	// Destructor
	~Scene(void) {
		glDeleteFramebuffers(1, &m_forward_fbo);
		glDeleteTextures(1, &m_forward_texture_color);
		glDeleteTextures(1, &m_forward_texture_depth);

		glDeleteFramebuffers(1, &m_deferred_fbo);
		glDeleteTextures(1, &m_deferred_texture_depth);
		glDeleteTextures(1, &m_deferred_texture_albedo);
		glDeleteTextures(1, &m_deferred_texture_specular);
		glDeleteTextures(1, &m_deferred_texture_emission);
		glDeleteTextures(1, &m_deferred_texture_position_wcs);
		glDeleteTextures(1, &m_deferred_texture_position_lcs);
		glDeleteTextures(1, &m_deferred_texture_normal);

		glDeleteBuffers(1, &m_ubo_matrices);
		glDeleteBuffers(1, &m_ubo_material);

		SAFE_DELETE(m_model);
		SAFE_DELETE(m_camera);
		SAFE_DELETE(m_light);

		SAFE_DELETE(m_root_models);
		SAFE_DELETE(m_root_lights);

		SAFE_DELETE(m_shader_basic_geometry);
		SAFE_DELETE(m_shader_spotlight);
		SAFE_DELETE(m_shader_shadow_map);
		SAFE_DELETE(m_shader_deferred_gbuffer);
		SAFE_DELETE(m_shader_deferred_lighting);
		SAFE_DELETE(m_shader_texture_depth);
		SAFE_DELETE(m_shader_texture_color);
		SAFE_DELETE(m_shader_texture_normal);
	};

	void Init(void);
	void Draw(void);
	void DrawSceneToFBO(void);
	void ForwardRendering(void);
	void DeferredRendering(void);

	bool CreateFBO_Forward(void);
	bool CreateFBO_Deferred(void);
	bool LoadObjModels(void);
	bool CreateShaders(void);
	bool ReloadShaders(void);
};

#endif //SCENE_H

// eof ///////////////////////////////// class Scene