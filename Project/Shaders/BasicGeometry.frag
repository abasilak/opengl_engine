#version 330 core

uniform vec4					uniform_material_color;

layout(location = 0) out vec4	out_color;

void main(void)
{
	out_color = uniform_material_color;
}