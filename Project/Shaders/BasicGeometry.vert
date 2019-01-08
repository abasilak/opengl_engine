#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 uniform_mvp;

void main(void)
{
	gl_Position = uniform_mvp * vec4(position,1);
}