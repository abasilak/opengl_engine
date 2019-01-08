#include "Scene.h"			// - Header file for Scene
#include "Renderer.h"		// - Header file for our OpenGL functions

		MouseGL		mouse;
extern	WindowGL	window;

		Scene*		scene;

#ifdef DEBUG_TEXT_RENDERING
		TextRendering*	text_rendering;
#endif

		ScreenQuad*		m_screen_quad_output;
#ifdef DEBUG_BUFFERS
		ScreenQuad*		m_screen_quad_debug;
#endif

		GLint		RenderingGL::m_polygon_face		= GL_FRONT_AND_BACK;
		GLint		RenderingGL::m_polygon_mode		= GL_FILL;
		vec4		RenderingGL::m_background_color = vec4(0.1f, 0.1f, 0.1f, 1.0f);

// This init function is called before FREEGLUT goes into its main loop.
bool Init(void)   
{ 
	// Initialize GLEW (openGL extensions)
	// GLEW is a library which determines which OpenGL extensions are supported on each platform
	// For example, in windows we need this since Microsoft does not support anything beyond OpenGL 1.1 (look at the gl/gl.h header file)
	// The code below uses GLEW to include any extensions supported in this platform
	glewExperimental = GL_TRUE;
	GLuint err = glewInit();
	// skip GLEW bug in glewInit
	if (GLEW_OK != err)
	{
		Print("GLEW error %s", glewGetErrorString(err));
		return false;
	}
	else
		Print("Using GLEW %s", glewGetString(GLEW_VERSION));

	// check specifically for OpenGL support
	bool ogl_version_supported = (bool)(GLEW_GET_VAR(__GLEW_VERSION_4_5)!=0);
	const string s = "OpenGL " + to_string(window.gl_major_version) + "." + to_string(window.gl_minor_version);
	if (!ogl_version_supported)
	{
		Print((s + " not supported. Exiting.").c_str());
		return false;
	}
	else
		Print((s + " supported.").c_str());

#if defined(_DEBUG) && defined(DEBUG_CONTEXT)
	if (GLEW_ARB_debug_output)
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		GLuint ids[3] = { 131185, 131218, 102 };
		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 3, ids, GL_FALSE);
		glDebugMessageCallbackARB(&DebugGLOutputToFile, NULL);
	}
#endif // _DEBUG

#ifdef DEBUG_TEXT_RENDERING
	text_rendering = new TextRendering();
	text_rendering->InitFT();
	text_rendering->InitVAO();
	if (!text_rendering->CreateShader())
	{
		Print("Error in loading shaders. Exiting");
		return false;
	}
#endif

	scene		= new Scene();
	// define the shaders here
	if (!scene->CreateShaders())
	{
		Print("Error in loading shaders. Exiting");
		return false;
	}

	// define the models here
	scene->m_model = new Model("knossos");
	if (!scene->LoadObjModels())
	{
		Print("Error in loading obj models. Exiting");
		return false;
	}
	scene->Init();

	scene->CreateFBO_Forward();
	scene->CreateFBO_Deferred();

	m_screen_quad_output = new ScreenQuad(1);
	m_screen_quad_output->SetViewport(window.m_viewport->m_width, window.m_viewport->m_height);
	m_screen_quad_output->AddShader(scene->m_shader_texture_color);
	m_screen_quad_output->AddTextureList({ scene->m_forward_texture_color }, {"Final Image"} );

#ifdef DEBUG_BUFFERS
	m_screen_quad_debug = new ScreenQuad(4);
	m_screen_quad_debug->SetViewport(window.m_viewport->m_width, window.m_viewport->m_height);
	// 1
	m_screen_quad_debug->AddShader(scene->m_shader_texture_depth);
	m_screen_quad_debug->AddTextureList({ scene->m_light->m_shadow_map_texture }, { "Shadow Map" });
	// 2
	m_screen_quad_debug->AddShader(scene->m_shader_texture_depth);
	m_screen_quad_debug->AddTextureList({ scene->m_deferred_texture_depth }, { "Depth" });
	// 3
	m_screen_quad_debug->AddShader(scene->m_shader_texture_color);
	m_screen_quad_debug->AddTextureList({ scene->m_deferred_texture_position_wcs }, { "Pos WCS" });
	// 4
	m_screen_quad_debug->AddShader(scene->m_shader_texture_color);
	m_screen_quad_debug->AddTextureList({ scene->m_deferred_texture_position_lcs }, { "Pos LCS" });
	// 5
	m_screen_quad_debug->AddShader(scene->m_shader_texture_normal);
	m_screen_quad_debug->AddTextureList({ scene->m_deferred_texture_normal }, { "Normal" });
	// 6
	m_screen_quad_debug->AddShader(scene->m_shader_texture_color);
	m_screen_quad_debug->AddTextureList({ scene->m_deferred_texture_albedo }, { "Albedo" });
	// 7
	m_screen_quad_debug->AddShader(scene->m_shader_texture_color);
	m_screen_quad_debug->AddTextureList({ scene->m_deferred_texture_specular }, { "Specular" });
	// 8
	m_screen_quad_debug->AddShader(scene->m_shader_texture_color);
	m_screen_quad_debug->AddTextureList({ scene->m_deferred_texture_emission }, { "Emission" });
#endif

	scene->m_deferred_light_shading->SetViewport(window.m_viewport->m_width, window.m_viewport->m_height);
	scene->m_deferred_light_shading->AddShader(scene->m_shader_deferred_lighting);
	scene->m_deferred_light_shading->AddTextureList({ 
		scene->m_deferred_texture_position_wcs, 
		scene->m_deferred_texture_position_lcs, 
		scene->m_deferred_texture_normal, 
		scene->m_deferred_texture_albedo, 
		scene->m_deferred_texture_specular, 
		scene->m_deferred_texture_emission, 
		scene->m_light->m_shadow_map_texture }, { "GBuffer Data" });

	// Enable the Depth Buffer
	DepthGL::EnableTest();
	DepthGL::Func(GL_LEQUAL);
	DepthGL::Clear(1.0f);
	DepthGL::EnableClamp();
	// Enable Face Culling
	CullingGL::Enable();
	CullingGL::Face(GL_BACK);
	CullingGL::Order(GL_CCW);
	// Set the Rendering Mode
	RenderingGL::SetMode();
	RenderingGL::ClearColor();
	RenderingGL::EnableSRGB();

	window.m_gpu_timer = new GPUTimerGL();

	glError();

	return true;
}

void Render(void)
{
	scene->Draw();

	m_screen_quad_output->Draw();

#ifdef DEBUG_TEXT_RENDERING
	text_rendering->Draw(window.m_viewport->m_width, window.m_viewport->m_height);
#endif

#ifdef DEBUG_BUFFERS
	m_screen_quad_debug->Draw();

#ifdef DEBUG_TEXT_RENDERING
	text_rendering->m_output->push_back(m_screen_quad_debug->GetTextureString());
	text_rendering->Draw(window.m_viewport->m_width / (m_screen_quad_debug->GetPercentage() / 1.5f), window.m_viewport->m_height / (m_screen_quad_debug->GetPercentage() / 1.5f));
#endif
#endif
	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case '1':
		scene->m_shading_type = Shading::Forward;
		break;
	case '2':
		scene->m_shading_type = Shading::Deferred;
		break;
	case 'l':
		scene->m_light->m_is_rendered = !scene->m_light->m_is_rendered;
		break;
	case 'S':
		scene->m_light->m_casts_shadows = !scene->m_light->m_casts_shadows;
		scene->m_light->UpdateUBO();
		break;
	case 'w':
		scene->m_camera->m_eye.z -= 1.0f;
		break;
	case 's':
		scene->m_camera->m_eye.z += 1.0f;
		break;
	case 'a':
		scene->m_camera->m_eye.x -= 1.0f;
		break;
	case 'd':
		scene->m_camera->m_eye.x += 1.0f;
		break;
	case 'q':
		scene->m_camera->m_eye.y += 1.0f;
		break;
	case 'e':
		scene->m_camera->m_eye.y -= 1.0f;
		break;
	case 'r':
		scene->ReloadShaders();
		break;
	case 'g':
		RenderingGL::change_mode();
		break;
#ifdef DEBUG_BUFFERS
	case '+':
		m_screen_quad_debug->Next();
		break;
	case '-':
		m_screen_quad_debug->Prev();
		break;
#endif
	case 32: // space
		window.m_pause = !window.m_pause;
		break;
	case 27: // escape
		glutLeaveMainLoop();
		return;
	default:
		return;
	}
	glutPostRedisplay();
}

void KeyboardSpecial(int key, int x, int y)
{
	switch(key)
	{
#ifdef DEBUG_TEXT_RENDERING 
	case GLUT_KEY_F1:
		text_rendering->m_enabled = !text_rendering->m_enabled;
		break;
#endif
#ifdef DEBUG_BUFFERS
	case GLUT_KEY_F2:
		m_screen_quad_debug->IncreasePercentage(window.m_viewport->m_width, window.m_viewport->m_height);
		break;
	case GLUT_KEY_F3:
		m_screen_quad_debug->DecreasePercentage(window.m_viewport->m_width, window.m_viewport->m_height);
		break;
#endif
	case GLUT_KEY_PAGE_DOWN:
		scene->m_camera->m_world_translate.y += 1.0f;
		scene->m_camera->ComputeWorldMatrix();
		break;
	case GLUT_KEY_PAGE_UP:
		scene->m_camera->m_world_translate.y -= 1.0f;
		scene->m_camera->ComputeWorldMatrix();
		break;
	case GLUT_KEY_DOWN:
		scene->m_camera->m_world_translate.z -= 1.0f;
		scene->m_camera->ComputeWorldMatrix();
		break;
	case GLUT_KEY_UP:
		scene->m_camera->m_world_translate.z += 1.0f;
		scene->m_camera->ComputeWorldMatrix();
		break;
	case GLUT_KEY_LEFT:
		scene->m_camera->m_world_translate.x += 1.0f;
		scene->m_camera->ComputeWorldMatrix();
		break;
	case GLUT_KEY_RIGHT:
		scene->m_camera->m_world_translate.x -= 1.0f;
		scene->m_camera->ComputeWorldMatrix();
		break;
	default:
		return;
	}
	glError();
}

void Mouse(int button, int state, int x, int y)
{
	//OutputDebugString("Entered Mouse\n");
	if (state == GLUT_DOWN)
	{
		mouse.m_prev_x = x;
		mouse.m_prev_y = y;
	}
	mouse.m_button = button;
	glError();
}

void MouseMotion(int x, int y)
{
	switch (mouse.m_button)
	{
	case GLUT_LEFT_BUTTON:
		scene->m_camera->m_target.x -= (x - mouse.m_prev_x) * 0.05f;
		scene->m_camera->m_target.y += (y - mouse.m_prev_y) * 0.05f;

		mouse.m_prev_x = x;
		mouse.m_prev_y = y;

		glutPostRedisplay();
		break;
	case GLUT_RIGHT_BUTTON:

		scene->m_camera->m_world_rotate_x += (x - mouse.m_prev_x) * 0.1f;
		scene->m_camera->ComputeWorldMatrix();

		mouse.m_prev_x = x;
		mouse.m_prev_y = y;

		glutPostRedisplay();
		break;
	default:
		break;
	}
	glError();
}

void Resize(int width, int height)
{ 
	// Hack to void zero height
	if (height == 0) height = 1;

	// keep these values globally to reset the viewport after rendering to an FBO
	window.m_viewport->m_width = width;
	window.m_viewport->m_height = height;

	scene->CreateFBO_Forward();
	scene->CreateFBO_Deferred();

	m_screen_quad_output->SetViewport(width, height);
	scene->m_deferred_light_shading->SetViewport(width, height);
#ifdef DEBUG_BUFFERS
	m_screen_quad_debug->SetViewport(width, height);
#endif

	// -------------------------------------------VIEWPORT----------------------------------------------//
	// Set up the viewport (the part of the window where the rendering happens)
	// 0, 0 are the coordinates at the LOWER LEFT corner of the viewport rectangle, in pixels
	// Width, height Specify the width and height of the viewport, in pixels.
	// Typically, this is the size of the window
	// This information will be used for the viewport transformation
	window.m_viewport->SetViewport();
	window.m_viewport->SetAspectRatio();
	// -------------------------------------------------------------------------------------------------//
	
	// --------------------------------PERSPECTIVE PROJECTION-------------------------------------------//
	// each time we resize the window, a new projection matrix must be defined
	scene->m_camera->PerspectiveProjection(window.m_viewport->m_aspect_ratio);
	// -------------------------------------------------------------------------------------------------//
	glError();
}

void Idle()
{
	glutPostRedisplay();
	//glutTimerFunc(ms, TimerSync, ms);
}

void TimerSync(int ms)
{
	//glutPostRedisplay();
	//glutTimerFunc(ms, TimerSync, ms);
}

void ReleaseGLUT()
{
	glError();

	SAFE_DELETE(m_screen_quad_output);
	glError();
#ifdef DEBUG_BUFFERS
	SAFE_DELETE(m_screen_quad_debug);
	glError();
#endif

	SAFE_DELETE (scene);
	glError();
}