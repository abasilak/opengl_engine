#ifndef MAIN_H
#define MAIN_H

#pragma once

struct MouseGL
{
	// Mouse button and coords
	GLint m_button = -1;
	GLint m_prev_x = 0;
	GLint m_prev_y = 0;
};

class ViewportGL
{
public:
	GLuint	m_width;
	GLuint	m_height;
	GLuint	m_left_corner_x;
	GLuint	m_left_corner_y;
	GLfloat	m_aspect_ratio;

	// Constructor
	ViewportGL(GLuint x, GLuint y, GLuint w, GLuint h) : m_left_corner_x(x), m_left_corner_y(y), m_width(w), m_height(h) { SetAspectRatio(); }
	~ViewportGL() { };

	void   SetAspectRatio(void) { m_aspect_ratio = (float)(m_width - m_left_corner_x) / (float)(m_height - m_left_corner_y); }
	void   SetViewport(void)	{ glViewport(m_left_corner_x, m_left_corner_y, m_width, m_height); }
};

class CullingGL
{
public:
	// Constructor
	CullingGL() { };

	// Destructor
	~CullingGL(void) {};

	static void Enable(void)			{ glEnable(GL_CULL_FACE); }
	static void Disable(void)			{ glDisable(GL_CULL_FACE); }
	static void Face(GLenum mode)		{ glCullFace(mode); }
	static void Order(GLenum mode)		{ glFrontFace(mode); }
};

class BlendingGL
{
public:
	// Constructor
	BlendingGL() { };

	// Destructor
	~BlendingGL(void) {};

	static void Enable(void)			{ glEnable(GL_BLEND); }
	static void Disable(void)			{ glDisable(GL_BLEND); }
	static void Func(	GLenum sfactor, 
						GLenum dfactor)	{ glBlendFunc(sfactor, dfactor); }
};

class GPUTimerGL
{
public:
	GLfloat		m_total_time;
	GLuint		m_timer_query;

	// Constructor
	GPUTimerGL() { glGenQueries(1, &m_timer_query); };

	// Destructor
	~GPUTimerGL(void) { glDeleteQueries(1, &m_timer_query); };

	void		ResetTotalTime	(void)		{ m_total_time = 0.0f; }
	void		AddTotalTime	(GLfloat t) { m_total_time += t; }

	void		Begin		(void)
	{ 
		glBeginQuery(GL_TIME_ELAPSED, m_timer_query); 
	}
	GLfloat		End(void)
	{ 
		glEndQuery(GL_TIME_ELAPSED); 

		return	GetResult() / 1000000.0f;
	}
	GLuint64	GetResult	(void)
	{
		GLint	 done = 0;
		GLuint64 elapsed_time;
		
		while (!done)
			glGetQueryObjectiv(m_timer_query, GL_QUERY_RESULT_AVAILABLE, &done);
		glGetQueryObjectui64v(m_timer_query, GL_QUERY_RESULT, &elapsed_time);

		return 	elapsed_time;
	}

};

class DepthGL
{
public:
	// Constructor
	DepthGL() { };

	// Destructor
	~DepthGL(void) {};

	static void EnableTest(void)		{ glEnable(GL_DEPTH_TEST); }
	static void DisableTest(void)		{ glDisable(GL_DEPTH_TEST); }
	static void EnableClamp(void)		{ glEnable(GL_DEPTH_CLAMP); }
	static void DisableClamp(void)		{ glDisable(GL_DEPTH_CLAMP); }
	static void EnableWrite(void)		{ glDepthMask(GL_TRUE); }
	static void DisableWrite(void)		{ glDepthMask(GL_FALSE); }
	static void Func(GLenum func)		{ glDepthFunc(func); }
	static void Clear(GLclampd depth)	{ glClearDepth(depth); glClear(GL_DEPTH_BUFFER_BIT); }
};

class RenderingGL
{
public:
	// public variable declarations
	static GLint		m_polygon_face; 
	static GLint		m_polygon_mode;
	static vec4			m_background_color;

	// public function declarations

	// Constructor
	RenderingGL() {};

	// Destructor
	~RenderingGL(void) {};

	static void EnableSRGB(void)		{ glEnable(GL_FRAMEBUFFER_SRGB); }
	static void DisableSRGB(void)		{ glDisable(GL_FRAMEBUFFER_SRGB); }
	static void SetMode(void)			{ glPolygonMode(m_polygon_face, m_polygon_mode); }
	static void SetModeScreen(void)		{ glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
	static void ClearColor(void)		{ glClearColor(m_background_color.r	, m_background_color.g, m_background_color.b, m_background_color.a); glClear(GL_COLOR_BUFFER_BIT); }
	static void ClearColor(vec4 color)	{ glClearColor(color.r				,			   color.g,				 color.b,			   color.a); glClear(GL_COLOR_BUFFER_BIT); }

	static void change_mode(void)	{
		if		(m_polygon_mode == GL_POINT)	m_polygon_mode = GL_LINE;
		else if (m_polygon_mode == GL_LINE)		m_polygon_mode = GL_FILL;
		else if (m_polygon_mode == GL_FILL)		m_polygon_mode = GL_POINT;

		SetMode();
	}

};

class WindowGL
{
public:
	// private variable declarations
	ViewportGL*		m_viewport;
	
	GPUTimerGL*		m_gpu_timer;

	GLboolean		m_pause;		
	GLuint			m_redisplay_time;		// (miliseconds)

	GLuint			m_width	 = 1024u;
	GLuint			m_height = 768u;

	const GLuint	gl_major_version = 4;
	const GLuint	gl_minor_version = 5;
	
	const uvec2		m_position = uvec2(0u, 0u); // left top corner
	const string	m_title = "OpenGL " + to_string(gl_major_version) + "." + to_string(gl_minor_version) + " Rendering";

	// Constructor
	WindowGL() : m_redisplay_time(16), m_pause(true)
	{ 
		m_viewport  = new ViewportGL(0u, 0u, m_width, m_height);
	};

	// Destructor
	~WindowGL(void) { SAFE_DELETE(m_viewport); };
};

#endif //MAIN_H

// eof ///////////////////////////////// 