//----------------------------------------------------//
//                                                    //
// File: Root.h										  //
// This scene graph is a basic example for the		  //
// object relational management of the scene          //
// This holds the root node							  //
//                                                    //
// Author:                                            //
// Kostas Vardis                                      //
//                                                    //
// These files are provided as part of the BSc course //
// of Computer Graphics at the Athens University of   //
// Economics and Business (AUEB)					  //
//                                                    //
//----------------------------------------------------//
#ifndef ROOT_H
#define ROOT_H

#pragma once
//using namespace

// includes ////////////////////////////////////////
#include "GroupNode.h"
#include "../Shaders.h"
#include "../Light.h"

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class Root : public GroupNode
{
protected:
	// protected variable declarations
	mat4x4							m_view_mat;
	mat4x4							m_projection_mat;

	Camera*							m_camera;

	SpotLight*						m_spotlight;
	TransformNode*					m_world_transform;

	Shader*							m_basic_geometry_shader;
	Shader*							m_spotlight_shader;
	Shader*							m_shadow_map_shader;
	Shader*							m_gbuffer_shader;

	GLuint							m_ubo_matrices;
	GLuint							m_ubo_material;

	mat4x4							m_light_view_mat;
	mat4x4							m_light_projection_mat;

	// protected function declarations

private:
	// private variable declarations
	void								SetRoot(GroupNode* gnd);

	// private function declarations


public:
	// Constructor
	Root(void);

	// Destructor
	~Root(void);

	// public function declarations
	void								Init(void);
	void								Update(void);
	void								Draw(int shader_type);

	GLuint								GetUBO_Matrices(void)						{ return m_ubo_matrices; }
	GLuint								GetUBO_Material(void)						{ return m_ubo_material; }
	void								SetUBO_Matrices(GLuint ubo)					{ m_ubo_matrices = ubo; }
	void								SetUBO_Material(GLuint ubo)					{ m_ubo_material = ubo; }

	// get functions
	mat4x4&								GetViewMat(void)							{ return m_view_mat; }
	mat4x4&								GetProjectionMat(void)						{ return m_projection_mat; }
	TransformNode*						GetWorldTransform(void)						{ return m_world_transform; }

	// set functions
	Camera*								GetActiveCamera(void)						{ return m_camera; }
	void								SetActiveCamera(Camera* camera)				{ m_camera = camera; }
	void								SetViewMat(glm::mat4x4& mat)				{ m_view_mat = mat; }
	void								SetProjectionMat(glm::mat4x4& mat)			{ m_projection_mat = mat; }

	// set light functions
	void								SetActiveSpotlight(SpotLight* light)		{ m_spotlight = light; }
	void								SetLightViewMat(glm::mat4x4& mat)			{ m_light_view_mat = mat; }
	void								SetLightProjectionMat(glm::mat4x4& mat)		{ m_light_projection_mat = mat; }

	// get light functions
	SpotLight*							GetActiveSpotlight(void)					{ return m_spotlight; }
	mat4x4&								GetLightViewMat(void)						{ return m_light_view_mat; }
	mat4x4&								GetLightProjectionMat(void)					{ return m_light_projection_mat; }

	// set shader functions
	void								SetNoLightingShader(Shader* shader)			{ m_basic_geometry_shader = shader; }
	void								SetSpotlightShader(Shader* shader)			{ m_spotlight_shader = shader; }
	void								SetShadowMapShader(Shader* shader)			{ m_shadow_map_shader = shader; }
	void								SetGBufferShader(Shader* shader)			{ m_gbuffer_shader = shader; }

	// get shader functions
	Shader*								GetNoLightingShader(void)					{ return m_basic_geometry_shader; }
	Shader*								GetSpotlightShader(void)					{ return m_spotlight_shader; }
	Shader*								GetShadowMapShader(void)					{ return m_shadow_map_shader; }
	Shader*								GetGBufferShader(void)						{ return m_gbuffer_shader; }
};

#endif //ROOT_H

// eof ///////////////////////////////// class Root