#version 330 core

layout (std140) uniform Material
{
	ivec4		uniform_material_has_tex;		// diffuse, normal, specular, emission
    vec4		uniform_material_diffuse_color; // .a = opacity
	vec4		uniform_material_specular_color;// .a = gloss
	vec3		uniform_material_emission_color;
};

struct Textures
{
	sampler2D	diffuse;
	sampler2D	normal;
	sampler2D	specular;
	sampler2D	emission;
}; 

uniform Textures		uniform_textures;

in VS_OUT
{
	vec3 position_wcs_v;	// the position 
	vec4 position_lcs_v;	// the position in light space 
    vec3 normal_wcs_v;		// the normal 
	vec2 texcoord_v;		// the texture coordinates
} fs_in;

layout(location = 0) out vec3 out_color_position_wcs;
layout(location = 1) out vec4 out_color_position_lcs;
layout(location = 2) out vec3 out_color_normal;
layout(location = 3) out vec4 out_color_albedo;
layout(location = 4) out vec4 out_color_specular;
layout(location = 5) out vec3 out_color_emission;

void main(void)
{
	// [0][POSITION][WCS]
	out_color_position_wcs = fs_in.position_wcs_v;

	// [1][POSITION][LCS]
	out_color_position_lcs = fs_in.position_lcs_v;

	// [2][NORMAL]
	out_color_normal	   = normalize(fs_in.normal_wcs_v);
	
	// [3][ALBEDO]
	vec4 color_albedo  = uniform_material_diffuse_color;
	if (uniform_material_has_tex.x > 0)
	{
		color_albedo   = texture(uniform_textures.diffuse, fs_in.texcoord_v.xy);
		if (color_albedo.a < 1.0f)
			discard;
	}
	out_color_albedo   = color_albedo;

	// [4][SPECULAR]
	vec4 color_specular = uniform_material_specular_color;
	if (uniform_material_has_tex.z > 0)
		color_specular = texture(uniform_textures.specular, fs_in.texcoord_v.xy);
	out_color_specular = color_specular;

	// [5][EMISSION]
	vec3 color_emission = uniform_material_emission_color;
	if (uniform_material_has_tex.w > 0)
		color_emission = texture(uniform_textures.emission, fs_in.texcoord_v.xy).rgb;
	out_color_emission = color_emission;
}
