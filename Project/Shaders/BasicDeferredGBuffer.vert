#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord0;
layout(location = 3) in vec2 texcoord1;
layout(location = 4) in vec3 tangent;

// attributes that are uniformly passed to all shaderers
layout (std140) uniform Matrices
{
    mat4 uniform_m;
    mat4 uniform_v;
	mat4 uniform_p;
	mat4 uniform_n;
	mat4 uniform_lpXlvXm;
};

// attributes that are passed to the fragment shader
out VS_OUT
{
	vec3 position_wcs_v;	// the position 
	vec4 position_lcs_v;	// the position in light space 
    vec3 normal_wcs_v;		// the normal 
	vec2 texcoord_v;		// the texture coordinates
} vs_out;

void main(void)
{
	// for shading from omni lights, we also need the current vertex in the fragment shader
	vs_out.position_wcs_v	= vec3(uniform_m * vec4(position, 1.0)).xyz;

	// for shadow mapping, we should transform current vertex to the light space
	vs_out.position_lcs_v	= uniform_lpXlvXm * vec4(position, 1.0);


	// transform the vertex normal with the normal matrix
	// we can do this in the fragment shader but since this is a per-vertex evaluation
	// so we do it in the vertex shader to save instructions
	vs_out.normal_wcs_v		= vec3(uniform_n * vec4(normal, 0.0)).xyz;

	// pass the texture coordinates
	vs_out.texcoord_v		= texcoord0;

	// vertex position in CSS
	gl_Position				= uniform_p * vec4((uniform_v * vec4(vs_out.position_wcs_v, 1.0)).xyz, 1);
}
