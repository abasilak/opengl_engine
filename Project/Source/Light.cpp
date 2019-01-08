#include "Scene.h"
#include "TextRendering.h"

extern Scene*	scene;
extern WindowGL	window;
#ifdef DEBUG_TEXT_RENDERING
extern TextRendering*	text_rendering;
#endif

bool SpotLight::CreateUBO(void)
{
	// the light position and target are transformed to ECS coordinates
	vec3	light_position_wcs = vec3(m_camera->m_eye);
	vec3	light_target_wcs = vec3(m_camera->m_target);
	// get the direction from the light position to the target (target - position)
	vec3	light_direction_wcs = normalize(light_target_wcs - light_position_wcs);

	vec3	light_ambient_color  = 0.01f * vec3(m_color);
	vec3	light_diffuse_color  = vec3(m_color);
	vec3	light_specular_color = vec3(m_color);
	vec3	light_attenuation_factors = vec3(m_att_constant, m_att_linear, m_att_quadratic);
	GLfloat light_spotlight_cutoff = cos(radians(m_spotlight_cutoff));

	glGenBuffers(1, &m_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
	{
		glBufferData   (GL_UNIFORM_BUFFER, 6 * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec4), value_ptr(vec4(m_camera->m_eye, m_is_spotlight ? 1.0f : 0.0f)));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec4), sizeof(vec4), value_ptr(vec4(light_direction_wcs, m_casts_shadows ? 1.0f : 0.0f)));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec4) * 2, sizeof(vec4), value_ptr(light_ambient_color));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec4) * 3, sizeof(vec4), value_ptr(light_diffuse_color));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec4) * 4, sizeof(vec4), value_ptr(light_specular_color));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec4) * 5, sizeof(vec4), value_ptr(vec4(light_attenuation_factors, light_spotlight_cutoff)));
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_ubo);

	return true;
}

void SpotLight::UpdateUBO(void)
{
	// get the direction from the light position to the target (target - position)
	vec3	light_direction_wcs = normalize(m_camera->m_target - m_camera->m_eye);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
	{
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec4), value_ptr(vec4(m_camera->m_eye, m_is_spotlight ? 1.0f : 0.0f)));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(vec4), sizeof(vec4), value_ptr(vec4(light_direction_wcs, m_casts_shadows ? 1.0f : 0.0f)));
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

bool SpotLight::CreateShadowFBO(void)
{
	// Create Shadow Texture
	glGenTextures(1, &m_shadow_map_texture);
	glBindTexture(GL_TEXTURE_2D, m_shadow_map_texture);
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_shadow_map_viewport->m_width, m_shadow_map_viewport->m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Create Shadow Framebuffer
	glGenFramebuffers(1, &m_shadow_map_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_map_fbo);
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadow_map_texture, 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glFrameBufferError("(Depth) Incomplete SpotLight fbo")) return false;
	Print("%s, Generated Depth Texture: %i.", m_name.c_str(), m_shadow_map_texture);

	return true;
}

void SpotLight::DrawSceneToShadowFBO()
{
	window.m_gpu_timer->Begin();

	// switch the rendering to happen on the FBO rather than the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_map_fbo);
	{
		m_shadow_map_viewport->SetViewport();
		DepthGL::Clear(1.0f);
		//CullingGL::Face(GL_FRONT);
		
		// disable color
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		// Create the light's world to view space matrix
		// we need to build a "camera" as viewed from the light
		// so we need an up vector, a target and a light "eye" position
		// create the direction vector
		vec3 light_direction = normalize(m_camera->m_target - m_camera->m_eye);

		// this check is simply a sanity check for the internal cross product in glm::lookAt
		// just in case the light direction vector is 0,1,0
		// if it is, the up vector is set to 0,0,1
		if (fabs(light_direction.z) < 0.001f && fabs(light_direction.x) < 0.001f)
			m_camera->m_up = vec3(0, 0, 1);
		else
			m_camera->m_up = vec3(0, 1, 0);

		// construct the light view matrix that transforms world space to light view space (WCS -> LCS)
		// LCS is the view space of the light, similar to ECS which is the view space for the camera
		m_camera->LookAt();

		//float h = spotlight->m_near_range *glm::tan(glm::radians(spotlight->m_aperture * 0.5f));
		//glm::mat4x4 light_projection_matrix = glm::frustum(-h, h, -h, h, spotlight->m_near_range, spotlight->m_far_range);
		// aspect ratio is 1 since both width and height are the same (dimensions of the texture)
		m_camera->PerspectiveProjection(m_shadow_map_viewport->m_aspect_ratio);

		// now draw the scene as usual

		// USE SCENE GRAPH
		scene->m_root_models->SetLightViewMat(m_camera->m_world_to_camera_matrix);
		scene->m_root_models->SetLightProjectionMat(m_camera->m_perspective_projection_matrix);
		scene->m_root_models->Draw(2);

		// enable color
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		//CullingGL::Face(GL_BACK);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLfloat rendering_time = window.m_gpu_timer->End();
	window.m_gpu_timer->AddTotalTime(rendering_time);
#ifdef DEBUG_TEXT_RENDERING
	text_rendering->m_output->push_back("[Shadow Mapping] [" + to_string(rendering_time) + "ms]");
#endif
}

void SpotLight::Animation(void)
{
	m_is_animated = true;

	m_rotationY += 0.3f; if (m_rotationY > 360.0f) m_rotationY -= 360.0f;
	m_camera->m_eye = vec3(rotate(radians(m_rotationY), vec3(0.0f, 1.0f, 0.0f)) * vec4(m_initial_position, 1.0f));
	
	UpdateUBO();

	// for sphere rendering
	m_world_transform->SetTranslation(m_camera->m_eye.x, m_camera->m_eye.y, m_camera->m_eye.z);
}