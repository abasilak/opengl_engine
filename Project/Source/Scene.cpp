#include "Scene.h" 
#include "TextRendering.h"

extern WindowGL			window;
#ifdef DEBUG_TEXT_RENDERING
extern TextRendering*	text_rendering;
#endif

void Scene::Init()
{
	// camera eye settings 
	m_camera->m_eye = vec3(60.0f, 100.0f, 170.0f);

	// light settings
	m_light->m_initial_target = vec3(0.0f, 0.0f, 0.0f);
	m_light->m_initial_position = vec3(60.0f, 30.0f, 0.0f);
	m_light->m_camera->m_eye = m_light->m_initial_position;
	m_light->m_world_transform->SetTranslation(m_light->m_camera->m_eye.x, m_light->m_camera->m_eye.y, m_light->m_camera->m_eye.z);
	m_light->CreateUBO();

	// Uniform Buffer Objects
	glGenBuffers(1, &m_ubo_matrices);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_matrices);
	{
		glBufferData(GL_UNIFORM_BUFFER, 5 * sizeof(mat4x4), NULL, GL_DYNAMIC_DRAW);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_ubo_matrices);

	glGenBuffers(1, &m_ubo_material);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ubo_material);
	{
		glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_ubo_material);

	// scene settings
	m_root_models->AddChild(m_root_models->GetWorldTransform());
	{
		m_root_models->GetWorldTransform()->AddChild(m_model->m_world_transform);
		m_model->m_world_transform->AddChild(m_model->m_geometry);
	}
	m_root_models->Init();
	m_root_models->SetUBO_Matrices(m_ubo_matrices);
	m_root_models->SetUBO_Material(m_ubo_material);
	m_root_models->SetActiveCamera(m_camera);
	m_root_models->SetActiveSpotlight(m_light);
	m_root_models->SetNoLightingShader(m_shader_basic_geometry);
	m_root_models->SetSpotlightShader(m_shader_spotlight);
	m_root_models->SetShadowMapShader(m_shader_shadow_map);
	m_root_models->SetGBufferShader(m_shader_deferred_gbuffer);

	m_root_lights->AddChild(m_root_lights->GetWorldTransform());
	{
		m_root_lights->GetWorldTransform()->AddChild(m_light->m_world_transform);
		m_light->m_world_transform->AddChild(m_light->m_geometry);
	}
	m_root_lights->Init();
	m_root_lights->SetNoLightingShader(m_shader_basic_geometry);
}

bool Scene::CreateFBO_Forward(void)
{
	// Texture Depth
	if (glIsTexture(m_forward_texture_depth))
		glDeleteTextures(1, &m_forward_texture_depth);
	glGenTextures(1, &m_forward_texture_depth);
	glBindTexture(GL_TEXTURE_2D, m_forward_texture_depth);
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, window.m_viewport->m_width, window.m_viewport->m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Texture Color
	if (glIsTexture(m_forward_texture_color))
		glDeleteTextures(1, &m_forward_texture_color);
	glGenTextures(1, &m_forward_texture_color);
	glBindTexture(GL_TEXTURE_2D, m_forward_texture_color);
	{
#ifdef TONE_MAPPING
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window.m_viewport->m_width, window.m_viewport->m_height, 0, GL_RGBA, GL_FLOAT, NULL);
#else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window.m_viewport->m_width, window.m_viewport->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
#endif
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// FBO
	if (glIsFramebuffer(m_forward_fbo))
		glDeleteFramebuffers(1, &m_forward_fbo);
	glGenFramebuffers(1, &m_forward_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_forward_fbo);
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_forward_texture_depth, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_forward_texture_color, 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glFrameBufferError("Incomplete Forward fbo")) return false;
	Print("Forward Rendering, Generated Depth Texture: %i.", m_forward_texture_depth);
	Print("Forward Rendering, Generated Color Texture: %i.", m_forward_texture_color);

	return true;
}

bool Scene::CreateFBO_Deferred(void)
{
	// Texture Depth
	if (glIsTexture(m_deferred_texture_depth))
		glDeleteTextures(1, &m_deferred_texture_depth);
	glGenTextures(1, &m_deferred_texture_depth);
	glBindTexture(GL_TEXTURE_2D, m_deferred_texture_depth);
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, window.m_viewport->m_width, window.m_viewport->m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Texture Position WCS
	if (glIsTexture(m_deferred_texture_position_wcs))
		glDeleteTextures(1, &m_deferred_texture_position_wcs);
	glGenTextures(1, &m_deferred_texture_position_wcs);
	glBindTexture(GL_TEXTURE_2D, m_deferred_texture_position_wcs);
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window.m_viewport->m_width, window.m_viewport->m_height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Texture Position LCS
	if (glIsTexture(m_deferred_texture_position_lcs))
		glDeleteTextures(1, &m_deferred_texture_position_lcs);
	glGenTextures(1, &m_deferred_texture_position_lcs);
	glBindTexture(GL_TEXTURE_2D, m_deferred_texture_position_lcs);
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window.m_viewport->m_width, window.m_viewport->m_height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Texture Normal
	if (glIsTexture(m_deferred_texture_normal))
		glDeleteTextures(1, &m_deferred_texture_normal);
	glGenTextures(1, &m_deferred_texture_normal);
	glBindTexture(GL_TEXTURE_2D, m_deferred_texture_normal);
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window.m_viewport->m_width, window.m_viewport->m_height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Texture Albedo
	if (glIsTexture(m_deferred_texture_albedo))
		glDeleteTextures(1, &m_deferred_texture_albedo);
	glGenTextures(1, &m_deferred_texture_albedo);
	glBindTexture(GL_TEXTURE_2D, m_deferred_texture_albedo);
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window.m_viewport->m_width, window.m_viewport->m_height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Texture Specular
	if (glIsTexture(m_deferred_texture_specular))
		glDeleteTextures(1, &m_deferred_texture_specular);
	glGenTextures(1, &m_deferred_texture_specular);
	glBindTexture(GL_TEXTURE_2D, m_deferred_texture_specular);
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window.m_viewport->m_width, window.m_viewport->m_height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Texture Emission
	if (glIsTexture(m_deferred_texture_emission))
		glDeleteTextures(1, &m_deferred_texture_emission);
	glGenTextures(1, &m_deferred_texture_emission);
	glBindTexture(GL_TEXTURE_2D, m_deferred_texture_emission);
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.m_viewport->m_width, window.m_viewport->m_height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// FBO
	if (glIsFramebuffer(m_deferred_fbo))
		glDeleteFramebuffers(1, &m_deferred_fbo);
	glGenFramebuffers(1, &m_deferred_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_deferred_fbo);
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT , GL_TEXTURE_2D, m_deferred_texture_depth, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_deferred_texture_position_wcs, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_deferred_texture_position_lcs, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_deferred_texture_normal, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_deferred_texture_albedo, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_deferred_texture_specular, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, m_deferred_texture_emission, 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glFrameBufferError("Incomplete Deferred fbo")) return false;
	Print("Deferred Rendering, Generated Depth Texture: %i.", m_deferred_texture_depth);
	Print("Deferred Rendering, Generated Position WCS Texture: %i.", m_deferred_texture_position_wcs);
	Print("Deferred Rendering, Generated Position LCS Texture: %i.", m_deferred_texture_position_lcs);
	Print("Deferred Rendering, Generated Normal Texture: %i.", m_deferred_texture_normal);
	Print("Deferred Rendering, Generated Albedo Texture: %i.", m_deferred_texture_albedo);
	Print("Deferred Rendering, Generated Specular Texture: %i.", m_deferred_texture_specular);
	Print("Deferred Rendering, Generated Emission Texture: %i.", m_deferred_texture_emission);

	return true;
}

void Scene::Draw()
{
	window.m_gpu_timer->ResetTotalTime();

	// Animate Lights
	if (!window.m_pause)
		m_light->Animation();

	// Shadow Mapping
	if (m_light->m_is_animated)
	{
		m_light->m_is_animated = false;
		m_light->DrawSceneToShadowFBO();
	}

	// Scene Rendering
	DrawSceneToFBO();

#ifdef DEBUG_TEXT_RENDERING
	text_rendering->m_output->push_back("[Total Time] [" + to_string(window.m_gpu_timer->m_total_time) + "ms]");
#endif
}

void Scene::ForwardRendering()
{
	GLfloat rendering_time;
#ifdef DEBUG_TEXT_RENDERING
	text_rendering->m_output->push_back("[Forward Rendering]");
#endif

	window.m_gpu_timer->Begin();
	glBindFramebuffer(GL_FRAMEBUFFER, m_forward_fbo);
	{
		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);

		window.m_viewport->SetViewport();
		RenderingGL::ClearColor();
		DepthGL::Clear(1.0f);
		{
			m_camera->LookAt();
			m_root_models->SetViewMat(m_camera->m_world_to_camera_matrix);
			m_root_models->SetProjectionMat(m_camera->m_perspective_projection_matrix);
			m_root_models->Draw(0);	// Phong Shading with Shadow Mapping 

			if (m_light->m_is_rendered)
			{
				DepthGL::DisableWrite();
				RenderingGL::SetModeScreen();
				m_root_lights->SetViewMat(m_camera->m_world_to_camera_matrix);
				m_root_lights->SetProjectionMat(m_camera->m_perspective_projection_matrix);
				m_root_lights->Draw(1);	// No Lighting 
				RenderingGL::SetMode();
				DepthGL::EnableWrite();
			}
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	rendering_time		  = window.m_gpu_timer->End();
	window.m_gpu_timer->AddTotalTime(rendering_time);

#ifdef DEBUG_TEXT_RENDERING
	text_rendering->m_output->push_back("[Forward Shading] [" + to_string(rendering_time) + "ms]");
	
#endif
}

void Scene::DeferredRendering()
{
	GLfloat rendering_time, total_rendering_time = 0.0f;
#ifdef DEBUG_TEXT_RENDERING
	text_rendering->m_output->push_back("[Deferred Rendering]");
#endif

	// G-Buffer Pass
	window.m_gpu_timer->Begin();
	glBindFramebuffer(GL_FRAMEBUFFER, m_deferred_fbo);
	{
		unsigned int attachments_deferred[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
		glDrawBuffers(5, attachments_deferred);

		window.m_viewport->SetViewport();
		RenderingGL::ClearColor(vec4(0.0f));
		DepthGL::Clear(1.0f);
		{
			m_camera->LookAt();
			m_root_models->SetViewMat(m_camera->m_world_to_camera_matrix);
			m_root_models->SetProjectionMat(m_camera->m_perspective_projection_matrix);
			m_root_models->Draw(3);	
		}
	}
	rendering_time		  = window.m_gpu_timer->End();
	window.m_gpu_timer->AddTotalTime(rendering_time);
#ifdef DEBUG_TEXT_RENDERING
	text_rendering->m_output->push_back("[G-Buffer Creation] [" + to_string(rendering_time) + "ms]");
#endif

	// Light Shading Pass
	window.m_gpu_timer->Begin();
	glBindFramebuffer(GL_FRAMEBUFFER, m_forward_fbo);
	{
		unsigned int attachments_forward[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments_forward);
		
		window.m_viewport->SetViewport();
		RenderingGL::ClearColor();
		DepthGL::Clear(1.0f);
		DepthGL::DisableWrite();

		m_deferred_light_shading->Draw();

		if (m_light->m_is_rendered)
		{
			RenderingGL::SetModeScreen();
			m_root_lights->SetViewMat(m_camera->m_world_to_camera_matrix);
			m_root_lights->SetProjectionMat(m_camera->m_perspective_projection_matrix);
			m_root_lights->Draw(1);	// No Lighting 
			RenderingGL::SetMode();
		}

		DepthGL::EnableWrite();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	rendering_time		  = window.m_gpu_timer->End();
	window.m_gpu_timer->AddTotalTime(rendering_time);
#ifdef DEBUG_TEXT_RENDERING
	text_rendering->m_output->push_back("[Light Shading] [" + to_string(rendering_time) + "ms]");
#endif
}

void Scene::DrawSceneToFBO()
{
	if		(m_shading_type == Shading::Forward)
		ForwardRendering();
	else // (m_shading_type == Shading::Deferred)
		DeferredRendering();
}

bool Scene::LoadObjModels()
{
	OBJLoader* objLoader = new OBJLoader();

	m_light->m_mesh = objLoader->loadMesh("sphere_white.obj", "..\\..\\Data\\Other", true);
	m_light->m_world_transform = new TransformNode("spotlight_transform");
	m_light->m_geometry = new GeometryNode("spotlight_geometry", m_light->m_mesh);

	m_model->m_mesh = objLoader->loadMesh("knossos.obj", "..\\..\\Data\\Knossos", true);
	m_model->m_world_transform = new TransformNode("knossos_transform");
	m_model->m_geometry = new GeometryNode("knossos_geometry", m_model->m_mesh);

	SAFE_DELETE (objLoader);

	return true;
}

bool Scene::CreateShaders()
{
	// Basic Geometry shader
	m_shader_basic_geometry = new GeometryShader("BasicGeometry");
	if (!m_shader_basic_geometry->LoadAndCompile())
		return false;
	m_shader_basic_geometry->Init();

	// Spotlight light shader
	m_shader_spotlight = new SpotLightShader("BasicSpotLight");
	if (!m_shader_spotlight->LoadAndCompile())
		return false;
	m_shader_spotlight->Init();

	// Shadow Map shader
	m_shader_shadow_map = new ShadowMapShader("BasicShadow");
	if (!m_shader_shadow_map->LoadAndCompile())
		return false;
	m_shader_shadow_map->Init();

	// Texture Depth shader
	m_shader_texture_depth = new TextureDepthShader("BasicTextureDepth");
	if (!m_shader_texture_depth->LoadAndCompile())
		return false;
	m_shader_texture_depth->InitDepth(m_camera->m_near_field, m_camera->m_far_field);

	// Texture Color shader
#ifdef TONE_MAPPING
	m_shader_texture_color = new TextureColorShader("BasicTextureColorHDR");
#else
	m_shader_texture_color = new TextureColorShader("BasicTextureColor");
#endif
	if (!m_shader_texture_color->LoadAndCompile())
		return false;
	m_shader_texture_color->Init();

	// Texture Normal shader
	m_shader_texture_normal = new TextureColorShader("BasicTextureNormal");
	if (!m_shader_texture_normal->LoadAndCompile())
		return false;
	m_shader_texture_normal->Init();

	// Deferred GBuffer shader
	m_shader_deferred_gbuffer = new DeferredGBufferShader("BasicDeferredGBuffer");
	if (!m_shader_deferred_gbuffer->LoadAndCompile())
		return false;
	m_shader_deferred_gbuffer->Init();

	// Deferred Lighting shader
	m_shader_deferred_lighting = new DeferredLightingShader("BasicDeferredLighting");
	if (!m_shader_deferred_lighting->LoadAndCompile())
		return false;
	m_shader_deferred_lighting->Init();

	// all shaders loaded OK
	return true;
}

bool Scene::ReloadShaders()
{
	if (!m_shader_basic_geometry->LoadAndCompile())		return false;
	m_shader_basic_geometry->Init();

	if (!m_shader_spotlight->LoadAndCompile())	return false;
	m_shader_spotlight->Init();

	if (!m_shader_shadow_map->LoadAndCompile())	return false;
	m_shader_shadow_map->Init();

	if (!m_shader_deferred_gbuffer->LoadAndCompile())	return false;
	m_shader_deferred_gbuffer->Init();

	if (!m_shader_texture_depth->LoadAndCompile())	return false;
	m_shader_texture_depth->Init();

	if (!m_shader_texture_color->LoadAndCompile())	return false;
	m_shader_texture_color->Init();

	if (!m_shader_texture_normal->LoadAndCompile())	return false;
	m_shader_texture_normal->Init();

	if (!m_shader_deferred_lighting->LoadAndCompile())	return false;
	m_shader_deferred_lighting->Init();

	return true;
}