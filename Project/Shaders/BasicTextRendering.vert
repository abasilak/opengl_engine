#version 330 core

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

uniform mat4 uniform_p;

out vec2 TexCoords;

void main()
{
    TexCoords = vertex.zw;

	gl_Position = uniform_p * vec4(vertex.xy, 0.0, 1.0);
}