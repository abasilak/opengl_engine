#include "ScreenQuad.h"

void ScreenQuad::InitVAO(void)
{
	const GLfloat m_vertices[] = // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	{
		// Positions   // TexCoords
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	// Setup screen VAO
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	{
		GLuint m_vbo;
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		{
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), &m_vertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0); // ??
	}
	glBindVertexArray(0);
}

void ScreenQuad::Draw()
{
	RenderingGL::SetModeScreen();
	DepthGL::Clear(1.0f);
	m_viewport->SetViewport();

	glUseProgram(m_shaders[m_id]->m_program_id);
	{
		glBindVertexArray(m_vao);
		{
			for (int i = 0; i < m_textures_ids[m_id].size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, m_textures_ids[m_id][i]);
			}
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(0);
	}
	glUseProgram(0);

	RenderingGL::SetMode();
}

void ScreenQuad::IncreasePercentage(GLuint width, GLuint height)
{
	GLuint new_window_percentage = m_window_percentage + 1;
	if (new_window_percentage <= width || new_window_percentage <= height)
	{
		m_window_percentage = new_window_percentage;
		SetViewport(width, height);
	}	
}

void ScreenQuad::DecreasePercentage(GLuint width, GLuint height)
{
	GLuint new_window_percentage = m_window_percentage - 1;
	if (new_window_percentage > 0u)
	{
		m_window_percentage = new_window_percentage;
		SetViewport(width, height);
	}
}

void ScreenQuad::SetViewport(GLuint width, GLuint height)
{
	GLuint left_corner_x = width - width / m_window_percentage;
	GLuint left_corner_y = height - height / m_window_percentage;
	GLuint right_corner_x = width / m_window_percentage;
	GLuint right_corner_y = height / m_window_percentage;
	m_viewport = new ViewportGL(left_corner_x, left_corner_y, right_corner_x, right_corner_y);
}

// eof ///////////////////////////////// class SCREEN_QUAD