#ifndef LIGHT_H
#define LIGHT_H

#pragma once

#include "HelpLib.h"    // - Library for including GL libraries, checking for OpenGL errors, writing to Output window, etc.
#include "Camera.h"
#include "Main.h"			// - Header file for our Window functions
#include "OBJ/OGLMesh.h"
#include "SceneGraph/TransformNode.h"
#include "SceneGraph/GroupNode.h"
#include "SceneGraph/GeometryNode.h"

// Spotlights are lights which contain a position and a target (target - position specifies the light's direction)
class SpotLight
{
public:

	string						m_name;
	vec3						m_color;
	vec3						m_initial_target;
	vec3						m_initial_position;
	float						m_rotationY;

	OGLMesh*					m_mesh;
	TransformNode*				m_world_transform;
	GeometryNode*				m_geometry;

	Camera*						m_camera;

	GLuint						m_shadow_map_fbo;
	GLuint						m_shadow_map_texture;
	ViewportGL*					m_shadow_map_viewport;

	GLuint						m_ubo;
	
	GLboolean					m_is_animated;
	GLboolean					m_is_spotlight;
	GLboolean					m_is_rendered;
	GLboolean					m_casts_shadows;
	GLfloat						m_spotlight_cutoff;

	GLfloat						m_att_constant;
	GLfloat						m_att_linear;
	GLfloat						m_att_quadratic;

	SpotLight(string name) :
		m_name(name),
		m_initial_target(0,0,0),
		m_initial_position(0,10,0),
		m_color(1,1,1),
		m_rotationY(0),
		m_is_rendered(true),
		m_is_spotlight(true),
		m_casts_shadows(true),
		m_is_animated(true)
	{
		m_camera = new Camera();
		m_camera->m_fov		= 90.0f;

		m_att_constant		= 1.0f;
		m_att_linear		= 0.01f;
		m_att_quadratic		= 0.0032f;
		m_spotlight_cutoff	= 30.0f;

		m_shadow_map_viewport = new ViewportGL(0u, 0u, 1024u, 1024u);

		CreateShadowFBO();
	}

	~SpotLight(void) {
		SAFE_DELETE (m_camera);
		SAFE_DELETE (m_mesh);
		SAFE_DELETE (m_geometry);
		SAFE_DELETE (m_world_transform);
		SAFE_DELETE (m_shadow_map_viewport);

		glDeleteBuffers		(1, &m_ubo);
		glDeleteFramebuffers(1, &m_shadow_map_fbo);
		glDeleteTextures	(1, &m_shadow_map_texture);
	};

	void Animation(void);
	bool CreateUBO(void);
	void UpdateUBO(void);
	bool CreateShadowFBO(void);
	void DrawSceneToShadowFBO(void);
};

#endif //LIGHT_H

// eof ///////////////////////////////// class Light