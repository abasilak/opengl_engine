//----------------------------------------------------//
//                                                    //
// File: GeometryNode.cpp							  //
// This scene graph is a basic example for the		  //
// object relational management of the scene          //
// This holds the geometry node which is used for     //
// rendering geometric data							  //
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
#include "../HelpLib.h"			// - Library for including GL libraries, checking for OpenGL errors, writing to Output window, etc.
#include "GeometryNode.h"		// - Header file for the GeometryNode class
#include "Root.h"				// - Header file for the Root class
#include "../OBJ/OGLMesh.h"		// - Header file for the OGLMesh class
#include "../OBJ/OBJMaterial.h"	// - Header file for the OBJMaterial class
#include "../OBJ/Texture.h"		// - Header file for the Texture class
#include "../ShaderGLSL.h"		// - Header file for GLSL objects

// defines /////////////////////////////////////////

GeometryNode::GeometryNode(const char* name, OGLMesh* ogl_mesh):
Node(name)
{
	m_ogl_mesh = ogl_mesh;
}

GeometryNode::~GeometryNode()
{

}

void GeometryNode::Init()
{
	Node::Init();
}

void GeometryNode::Update()
{
	Node::Update();
}

void GeometryNode::Draw(int shader_type)
{
	if (m_ogl_mesh == nullptr)
		return;

	// SHADER TYPE 0 - use spotlight shader
	// SHADER TYPE 1 - render to shadow map shader
	if (shader_type == 0)
	{
		DrawUsingSpotLight();
	}
	else if (shader_type == 1)
	{
		DrawUsingNolighting();
	}
	else if (shader_type == 2)
	{
		DrawToShadowMap();
	}
	else if (shader_type == 3)
	{
		DrawToGBuffer();
	}

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GeometryNode::DrawUsingNolighting()
{
	mat4x4& M = GetTransform();
	mat4x4& V = m_root->GetViewMat();
	mat4x4& P = m_root->GetProjectionMat();
	mat4x4  MVP = P * V * M;

	GeometryShader* shader = (GeometryShader*)m_root->GetNoLightingShader();
	glUseProgram(shader->m_program_id);
	{
		glUniform4f(glGetUniformLocation(shader->m_program_id, "uniform_material_color"), 1.0f, 1.0f, 0.0f, 1.0f); // yellow
		glUniformMatrix4fv(shader->m_uniform_mvp, 1, false, &MVP[0][0]);

		glBindVertexArray(m_ogl_mesh->vao);
		{
			// loop through all the elements
			for (GLint i = 0; i < m_ogl_mesh->num_elements; i++)
			{
				if (m_ogl_mesh->elements[i].triangles == 0)
					continue;

				glDrawRangeElements(
					GL_TRIANGLES,
					m_ogl_mesh->elements[i].start_index,
					m_ogl_mesh->elements[i].start_index + m_ogl_mesh->elements[i].triangles * 3,
					m_ogl_mesh->elements[i].triangles * 3,
					GL_UNSIGNED_INT,
					(void*)(m_ogl_mesh->elements[i].start_index*sizeof(GLuint))
					);
			}
		}
		glBindVertexArray(0);
	}
	glUseProgram(0);
}

void GeometryNode::DrawUsingSpotLight()
{
	SpotLight* light = m_root->GetActiveSpotlight();
	if (light == nullptr) return;

	Camera* camera = m_root->GetActiveCamera();

	mat4x4& M = GetTransform();
	mat4x4& N = GetNormalMatrix();
	mat4x4& V = m_root->GetViewMat();
	mat4x4& P = m_root->GetProjectionMat();

	mat4x4& LV = m_root->GetLightViewMat();
	mat4x4& LP = m_root->GetLightProjectionMat();
	mat4x4& LPxLVxM = LP * LV * M;

	GLuint  UBO_Matrices = m_root->GetUBO_Matrices();
	GLuint  UBO_Material = m_root->GetUBO_Material();

	SpotLightShader* shader = (SpotLightShader*)m_root->GetSpotlightShader();

	glBindBuffer(GL_UNIFORM_BUFFER, UBO_Matrices);
	{
		glBufferSubData(GL_UNIFORM_BUFFER, 0				 , sizeof(mat4x4), value_ptr(M));
		glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(mat4x4), sizeof(mat4x4), value_ptr(V));
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4x4), sizeof(mat4x4), value_ptr(P));
		glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(mat4x4), sizeof(mat4x4), value_ptr(N));
		glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(mat4x4), sizeof(mat4x4), value_ptr(LPxLVxM));
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glUseProgram(shader->m_program_id);
	{
		// 1. set the camera
		vec4 camera_position_wcs = camera->m_world_matrix * vec4(camera->m_eye, 1.0f);
		glUniform3f(shader->m_uniform_camera_position_wcs, camera_position_wcs.x, camera_position_wcs.y, camera_position_wcs.z);
	
		// pass the shadow map sampler and texture
		// activate texture unit 4 for the shadow map texture
		// we can do this here instead of the for loop since
		// this does not change within the for loop
		glActiveTexture(GL_TEXTURE4);
		// bind the depth texture to the active texture unit
		glBindTexture(GL_TEXTURE_2D, light->m_shadow_map_texture);
		// send the sampler to the shader
		//glUniform1i(shader->m_uniform_textures_shadow_map, 4);

		// bind the VAO
		glBindVertexArray(m_ogl_mesh->vao);
		{
			// loop through all the elements
			for (GLint i = 0; i < m_ogl_mesh->num_elements; i++)
			{
				if (m_ogl_mesh->elements[i].triangles == 0)
					continue;

				// Material and texture goes here.
				int mtrIdx = m_ogl_mesh->elements[i].material_index;
				OBJMaterial& cur_material = *m_ogl_mesh->materials[mtrIdx];

				ivec4	material_has_tex = ivec4(cur_material.m_diffuse_opacity_tex_loaded, cur_material.m_normal_tex_loaded, cur_material.m_specular_gloss_tex_loaded, cur_material.m_emission_tex_loaded);

				glBindBuffer(GL_UNIFORM_BUFFER, UBO_Material);
				{
					glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ivec4), value_ptr(material_has_tex));
					glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(GLint)					, sizeof(vec4), value_ptr(vec4(cur_material.m_diffuse, cur_material.m_opacity)));
					glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(GLint) + sizeof(vec4)		, sizeof(vec4), value_ptr(vec4(cur_material.m_specular, cur_material.m_gloss)));
					glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(GLint) + 2 * sizeof(vec4)	, sizeof(vec4), value_ptr(cur_material.m_emission));
				}
				glBindBuffer(GL_UNIFORM_BUFFER, 0);

				// load textures
				// On the C++ side, this process requires two steps for each texture
				// First: activate a texture unit and bind the texture to that texture unit 
				// Second: pass a uniform to the shader for each texture to be used as a "sampler". 
				// On the GPU side, you just need to access the texture by using the sampler uniform you created in the C++ side

				// check if diffuse texture is present
				if (!cur_material.m_diffuse_opacity_tex_file.empty())
				{
					// activate texture unit 0 for the diffuse texture
					glActiveTexture(GL_TEXTURE0);
					// bind the diffuse texture to the active texture unit
					// the first parameter is the target. for 2D textures we use GL_TEXTURE_2D
					// the second parameter is the value that was returned from glGenTextures
					glBindTexture(GL_TEXTURE_2D, cur_material.m_diffuse_opacity_tex->get_texture_gl_id());
				}

				// check if normal texture is present
				if (!cur_material.m_normal_tex_file.empty())
				{
					// activate texture unit 1 for the normal texture
					glActiveTexture(GL_TEXTURE1);
					// bind the normal texture to the active texture unit
					// the first parameter is the target. for 2D textures we use GL_TEXTURE_2D
					// the second parameter is the value that was returned from glGenTextures
					glBindTexture(GL_TEXTURE_2D, cur_material.m_normal_tex->get_texture_gl_id());
				}

				// check if specular texture is present
				if (!cur_material.m_specular_gloss_tex_file.empty())
				{
					// activate texture unit 2 for the specular texture
					glActiveTexture(GL_TEXTURE2);
					// bind the specular texture to the active texture unit
					// the first parameter is the target. for 2D textures we use GL_TEXTURE_2D
					// the second parameter is the value that was returned from glGenTextures
					glBindTexture(GL_TEXTURE_2D, cur_material.m_specular_gloss_tex->get_texture_gl_id());
				}

				// check if emission texture is present
				if (!cur_material.m_emission_tex_file.empty())
				{
					// activate texture unit 3 for the emission texture
					glActiveTexture(GL_TEXTURE3);
					// bind the emission texture to the active texture unit
					// the first parameter is the target. for 2D textures we use GL_TEXTURE_2D
					// the second parameter is the value that was returned from glGenTextures
					glBindTexture(GL_TEXTURE_2D, cur_material.m_emission_tex->get_texture_gl_id());
				}

				// draw within a range in the index buffer
				glDrawRangeElements(
					GL_TRIANGLES,
					m_ogl_mesh->elements[i].start_index,
					m_ogl_mesh->elements[i].start_index + m_ogl_mesh->elements[i].triangles * 3,
					m_ogl_mesh->elements[i].triangles * 3,
					GL_UNSIGNED_INT,
					(void*)(m_ogl_mesh->elements[i].start_index*sizeof(GLuint))
					);

				// set the texture units to not point to any textures
				// if we do not do this, then the texture units will point to the bound textures
				// until we set them again
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
		glBindVertexArray(0);
	}
	glUseProgram(0);
}

void GeometryNode::DrawToShadowMap()
{
	// get the world transformation (hierarchically)
	mat4x4& M = GetTransform();
	// get the light view transformation (NOTE: normally, this should come from a light node)
	mat4x4& LV = m_root->GetLightViewMat();
	// get the light projection transformation (NOTE: normally, this should come from a light node)
	mat4x4& LP = m_root->GetLightProjectionMat();

	// this transforms OCS->WCS->LCS->CSS;
	mat4x4  MVP = LP * LV * M;

	// get the shader from the root node
	ShadowMapShader* shader = (ShadowMapShader*)m_root->GetShadowMapShader();
	glUseProgram(shader->m_program_id);
	{
		glUniformMatrix4fv(shader->m_uniform_mvp, 1, false, &MVP[0][0]);

		glBindVertexArray(m_ogl_mesh->vao);
		{
			// loop through all the elements
			for (GLint i = 0; i < m_ogl_mesh->num_elements; i++)
			{
				if (m_ogl_mesh->elements[i].triangles == 0)
					continue;

				// draw within a range in the index buffer
				glDrawRangeElements(
					GL_TRIANGLES,
					m_ogl_mesh->elements[i].start_index,
					m_ogl_mesh->elements[i].start_index + m_ogl_mesh->elements[i].triangles * 3,
					m_ogl_mesh->elements[i].triangles * 3,
					GL_UNSIGNED_INT,
					(void*)(m_ogl_mesh->elements[i].start_index*sizeof(GLuint))
					);
			}
		}
		glBindVertexArray(0);
	}
	glUseProgram(0);
}

void GeometryNode::DrawToGBuffer()
{
	SpotLight* light = m_root->GetActiveSpotlight();
	if (light == nullptr) return;

	mat4x4& M = GetTransform();
	mat4x4& N = GetNormalMatrix();
	mat4x4& V = m_root->GetViewMat();
	mat4x4& P = m_root->GetProjectionMat();

	mat4x4& LV = m_root->GetLightViewMat();
	mat4x4& LP = m_root->GetLightProjectionMat();
	mat4x4& LPxLVxM = LP * LV * M;

	GLuint  UBO_Matrices = m_root->GetUBO_Matrices();
	GLuint  UBO_Material = m_root->GetUBO_Material();

	DeferredGBufferShader* shader = (DeferredGBufferShader*)m_root->GetGBufferShader();

	glBindBuffer(GL_UNIFORM_BUFFER, UBO_Matrices);
	{
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4x4), value_ptr(M));
		glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(mat4x4), sizeof(mat4x4), value_ptr(V));
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4x4), sizeof(mat4x4), value_ptr(P));
		glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(mat4x4), sizeof(mat4x4), value_ptr(N));
		glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(mat4x4), sizeof(mat4x4), value_ptr(LPxLVxM));
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	glUseProgram(shader->m_program_id);
	{
		// bind the VAO
		glBindVertexArray(m_ogl_mesh->vao);
		{
			// loop through all the elements
			for (GLint i = 0; i < m_ogl_mesh->num_elements; i++)
			{
				if (m_ogl_mesh->elements[i].triangles == 0)
					continue;

				// Material and texture goes here.
				int mtrIdx = m_ogl_mesh->elements[i].material_index;
				OBJMaterial& cur_material = *m_ogl_mesh->materials[mtrIdx];

				ivec4	material_has_tex = ivec4(cur_material.m_diffuse_opacity_tex_loaded, cur_material.m_normal_tex_loaded, cur_material.m_specular_gloss_tex_loaded, cur_material.m_emission_tex_loaded);
				
				glBindBuffer(GL_UNIFORM_BUFFER, UBO_Material);
				{
					glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ivec4), value_ptr(material_has_tex));
					glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(GLint), sizeof(vec4), value_ptr(vec4(cur_material.m_diffuse, cur_material.m_opacity)));
					glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(GLint) + sizeof(vec4), sizeof(vec4), value_ptr(vec4(cur_material.m_specular, cur_material.m_gloss)));
					glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(GLint) + 2 * sizeof(vec4), sizeof(vec4), value_ptr(cur_material.m_emission));
				}
				glBindBuffer(GL_UNIFORM_BUFFER, 0);

				// load textures
				// On the C++ side, this process requires two steps for each texture
				// First: activate a texture unit and bind the texture to that texture unit 
				// Second: pass a uniform to the shader for each texture to be used as a "sampler". 
				// On the GPU side, you just need to access the texture by using the sampler uniform you created in the C++ side

				// check if diffuse texture is present
				if (!cur_material.m_diffuse_opacity_tex_file.empty())
				{
					// activate texture unit 0 for the diffuse texture
					glActiveTexture(GL_TEXTURE0);
					// bind the diffuse texture to the active texture unit
					// the first parameter is the target. for 2D textures we use GL_TEXTURE_2D
					// the second parameter is the value that was returned from glGenTextures
					glBindTexture(GL_TEXTURE_2D, cur_material.m_diffuse_opacity_tex->get_texture_gl_id());
				}

				// check if normal texture is present
				if (!cur_material.m_normal_tex_file.empty())
				{
					// activate texture unit 1 for the normal texture
					glActiveTexture(GL_TEXTURE1);
					// bind the normal texture to the active texture unit
					// the first parameter is the target. for 2D textures we use GL_TEXTURE_2D
					// the second parameter is the value that was returned from glGenTextures
					glBindTexture(GL_TEXTURE_2D, cur_material.m_normal_tex->get_texture_gl_id());
				}

				// check if specular texture is present
				if (!cur_material.m_specular_gloss_tex_file.empty())
				{
					// activate texture unit 2 for the specular texture
					glActiveTexture(GL_TEXTURE2);
					// bind the specular texture to the active texture unit
					// the first parameter is the target. for 2D textures we use GL_TEXTURE_2D
					// the second parameter is the value that was returned from glGenTextures
					glBindTexture(GL_TEXTURE_2D, cur_material.m_specular_gloss_tex->get_texture_gl_id());
				}

				// check if emission texture is present
				if (!cur_material.m_emission_tex_file.empty())
				{
					// activate texture unit 3 for the emission texture
					glActiveTexture(GL_TEXTURE3);
					// bind the emission texture to the active texture unit
					// the first parameter is the target. for 2D textures we use GL_TEXTURE_2D
					// the second parameter is the value that was returned from glGenTextures
					glBindTexture(GL_TEXTURE_2D, cur_material.m_emission_tex->get_texture_gl_id());
				}

				// draw within a range in the index buffer
				glDrawRangeElements(
					GL_TRIANGLES,
					m_ogl_mesh->elements[i].start_index,
					m_ogl_mesh->elements[i].start_index + m_ogl_mesh->elements[i].triangles * 3,
					m_ogl_mesh->elements[i].triangles * 3,
					GL_UNSIGNED_INT,
					(void*)(m_ogl_mesh->elements[i].start_index*sizeof(GLuint))
					);

				// set the texture units to not point to any textures
				// if we do not do this, then the texture units will point to the bound textures
				// until we set them again
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
		glBindVertexArray(0);
	}
	glUseProgram(0);

	glError();
}

// eof ///////////////////////////////// class GeometryNode