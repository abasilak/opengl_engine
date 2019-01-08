#ifndef CAMERA_H
#define CAMERA_H

#pragma once

class Camera
{
private:
	// private variable declarations

	// private function declarations

public:
	// public variable declarations
	
	// camera parameters
	vec3	m_eye;
	vec3	m_target;
	vec3	m_up;

	GLfloat m_near_field;			// value for the near clipping plane. Anything before that will be clipped
	GLfloat m_far_field;			// value for the far clipping plane. Anything beyond that will be clipped
	GLfloat m_fov;					// vertical field of view angle

	float	m_world_rotate_x;
	vec3	m_world_translate;

	// transformation matrices
	mat4x4 m_world_matrix;
	mat4x4 m_world_to_camera_matrix;
	mat4x4 m_perspective_projection_matrix;

	// public function declarations

	// Constructor
	Camera()		 {
		m_eye = vec3(0.0f, 0.0f, 40.0f);
		m_target = vec3(0.0f, 0.0f, 0.0f);
		m_up = vec3(0.0f, 1.0f, 0.0f);

		m_near_field = 1.0f;
		m_far_field = 1000.0f;
		m_fov = radians(30.0f);

		m_world_rotate_x = 0.0f;
		m_world_translate = vec3(0.0f);
	};

	// Destructor
	~Camera(void) {};

	void ComputeWorldMatrix()
	{
		m_world_matrix = translate(m_world_translate) * rotate(radians(m_world_rotate_x), vec3(0.0f, 1.0f, 0.0f));
	}

	void LookAt()
	{
		m_world_to_camera_matrix = lookAt(m_eye, m_target, m_up) * m_world_matrix;
	}

	void PerspectiveProjection(float aspect_ratio)
	{
		m_perspective_projection_matrix = perspective(m_fov, aspect_ratio, m_near_field, m_far_field);
	}
};

#endif //CAMERA_H

// eof ///////////////////////////////// class Camera