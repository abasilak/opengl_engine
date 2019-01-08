#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texcoords;

// attributes that are passed to the fragment shader
out VS_OUT
{
	vec2 texcoord_v;		// the texture coordinates
} vs_out;

void main()
{
	// pass the texture coordinates
	vs_out.texcoord_v = texcoords;
    
	gl_Position = vec4(position.xy, 0.0f, 1.0f); 
}  