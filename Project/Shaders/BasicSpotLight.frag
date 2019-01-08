#version 330 core

#define SHADOW_MAPPING 
#define SHADOW_EARLY_BAILING
#define SHADOW_NUM_SAMPLES_EARLY	4
#define SHADOW_NUM_SAMPLES			16
#define SHADOW_DEPTH_BIAS			0.0005f
#define SHADOW_CONSTANT_BIAS
#define SHADOW_PCF
#define SHADOW_POISSON_SAMPLING
#define SHADOW_STRATIFIED_SAMPLING

#define POINT_LIGHT_LINEAR_ATT

#define BLINN_PHONG_SHADING

#ifndef PI
#define PI 3.1415936
#endif // PI

// Calculates the reflection vector
// Parameters:
// - V the incident direction, looking away from the point of interest
// - N: the normal vector, looking away from the point of interest
// Returns the reflection vector
vec3 calc_reflection_vector(vec3 V, vec3 N)
{
	return normalize((2.0*N*dot(N, V)) - V);
}

// The Phong BRDF for surface reflection
// Parameters:
// - L, the incident (light) direction
// - C, the outgoing (camera) direction
// - N, the normal vector
// - m, the roughness (width) parameter
// Returns the Phong BRDF
float phong_specular(vec3 L, vec3 C, vec3 N, float m)
{
	float	surface_smoothness	= (1.0 - m) * 127.0;
	vec3	R					= calc_reflection_vector(L, N);
	float	thdotv				= max(dot(R, C), 0.01);

	// normalized phong brdf
	return pow(thdotv, surface_smoothness) * (surface_smoothness + 2) / (2 * PI);
}

// The Blinn-Phong BRDF for surface reflection
// Parameters:
// - I, the incident (light) direction
// - O, the outgoing (camera) direction
// - N, the normal vector
// - m, the roughness (width) parameter
// Returns the Blinn-Phong BRDF
float phong_blinn_specular(vec3 L, vec3 V, vec3 N, float m)
{
	float surface_smoothness = (1.0 - m) * 127.0;
	vec3  H		= normalize(L + V);
	float hdotv = max(dot(H, N), 0.01);

	// normalized blinn-phong brdf
	return pow(hdotv, surface_smoothness) * (surface_smoothness + 8) / (8 * PI);
}

// The Lambert BRDF for body reflection
// Parameters:
// - diffuce_color: the material's color or the albedo of the surface, i.e., the amount of energy being reflected instead of absorbed.
// Returns the Lambert BRDF
vec3 Lambert_BRDF(vec3 diffuse_color)
{
	return diffuse_color.rgb / PI;
}

layout (std140) uniform Material
{
	ivec4		uniform_material_has_tex;		// diffuse, normal, specular, emission
    vec4		uniform_material_diffuse_color; // .a = opacity
	vec4		uniform_material_specular_color;// .a = gloss
	vec3		uniform_material_emission_color;
};

layout (std140) uniform Light
{
	vec4		uniform_light_position_wcs;	//.w == spot or directinal
	vec4		uniform_light_direction_wcs; //.w == cast shadows.

	vec3		uniform_light_ambient_color;
	vec3		uniform_light_diffuse_color;
	vec3		uniform_light_specular_color;

	vec4		uniform_light_attenuation_cutoff; 
};

struct Camera
{
	vec3		position_wcs;
};

struct Textures
{
	sampler2D	diffuse;
	sampler2D	normal;
	sampler2D	specular;
	sampler2D	emission;
	sampler2D	shadow_map;
}; 

uniform Camera			uniform_camera;
uniform Textures		uniform_textures;

in VS_OUT
{
	vec3 position_wcs_v;	// the position 
	vec4 position_lcs_v;	// the position in light space 
	vec3 normal_wcs_v;		// the normal 
	vec2 texcoord_v;		// the texture coordinates
} fs_in;

layout(location = 0) out vec4 out_color;

#ifdef SHADOW_POISSON_SAMPLING
vec2 poissonDisk[16] = vec2[]
( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);
#else
vec2 kernelDisk[16] = 
vec2[]
(
  vec2(  0 ,  0 ),
  vec2(  0 ,  1 ),
  vec2(  0 , -1 ),
  vec2(  1 ,  0 ),
  vec2( -1 ,  0 ),
  vec2(  1 ,  1 ),
  vec2( -1 ,  1 ),
  vec2(  1 , -1 ),
  vec2( -1 , -1 ),
  vec2(  0 ,  0.5 ),
  vec2(  0 , -0.5 ),
  vec2( 0.5,  0 ),
  vec2(-0.5,  0 ),
  vec2( 0.5,  0.5 ),
  vec2(-0.5,  0.5 ),
  vec2( 0.5, -0.5 )
);
#endif

// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i)
{
	vec4	seed4 = vec4(seed, i);
	float	dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return	fract(sin(dot_product) * 43758.5453);
}

#ifdef SHADOW_MAPPING
float is_point_in_shadow(vec4 position, float bias)
{
	return (position.z > texture(uniform_textures.shadow_map, position.xy).r + bias) ? 0.0f : 1.0f;
}

float is_point_in_shadow_pcf(vec4 position, float bias)
{
	int   total_samples = 0;
	float isInShadow	= 0.0f;
	float shadow_map_step = 1.0/float(textureSize(uniform_textures.shadow_map, 0));

	for (int i=0; i<SHADOW_NUM_SAMPLES; i++)
	{
#ifdef SHADOW_STRATIFIED_SAMPLING
		int index = int(16.0*random(gl_FragCoord.xyy, i)) % 16; 
#else
		int index = i;
#endif
		
#ifdef SHADOW_POISSON_SAMPLING
		float shadow_map_z = texture(uniform_textures.shadow_map, position.xy + poissonDisk[index]*shadow_map_step).r;
#else
		float shadow_map_z = texture(uniform_textures.shadow_map, position.xy + kernelDisk [index]*shadow_map_step).r;
#endif
		if (position.z < shadow_map_z + bias)
			isInShadow += 1.0f;

		total_samples++;
#ifdef SHADOW_EARLY_BAILING
		if(i == SHADOW_NUM_SAMPLES_EARLY-1 && (isInShadow == 0.0f || isInShadow == float(SHADOW_NUM_SAMPLES_EARLY)))
			break;
#endif
	}
	isInShadow /= float(total_samples);
	
	return isInShadow;
}

float lightGetShadow(const float diffuse_angle_factor)
{
	float	shadow_factor		=  0.0f;
	vec4	position_lcs		= fs_in.position_lcs_v / fs_in.position_lcs_v.w;
			position_lcs.xyz	= (position_lcs.xyz + vec3(1)) * 0.5f;
	
	// Light Frustum Culling
	if ((clamp(position_lcs.xy, vec2(0,0), vec2(1,1)) - position_lcs.xy) == vec2(0,0))
	{
#ifdef SHADOW_CONSTANT_BIAS
		float	bias = SHADOW_DEPTH_BIAS;
#else
		float	bias = SHADOW_DEPTH_BIAS*tan(acos(diffuse_angle_factor)); 
				bias = clamp(bias, 0, 0.001f);
#endif

#ifdef SHADOW_PCF
		shadow_factor = is_point_in_shadow_pcf	(position_lcs, bias);
#else
		shadow_factor = is_point_in_shadow		(position_lcs, bias);		
#endif
	}
	return shadow_factor;
}

#endif

float lightGetAttenuation(const float dist_to_light)
{
#ifdef POINT_LIGHT_LINEAR_ATT
	// linear attenuation
	float	attenuation_factor = 1.0f / (uniform_light_attenuation_cutoff.x + uniform_light_attenuation_cutoff.y * dist_to_light);
#else
	// quadratic attenuation
	float	attenuation_factor = 1.0f / (uniform_light_attenuation_cutoff.x + uniform_light_attenuation_cutoff.y * dist_to_light + uniform_light_attenuation_cutoff.z * dist_to_light * dist_to_light);
#endif

	return	attenuation_factor;
}

float lightGetSpot(const vec3 vertex_to_light_wcs, const vec3 light_direction_wcs)
{
	float	spotangle_angle		= max(0.0, dot(vertex_to_light_wcs	, light_direction_wcs.xyz));
	
	return (spotangle_angle > uniform_light_attenuation_cutoff.w) ?
		1.0 - (1.0 - spotangle_angle) * 1.0/(1.0 - uniform_light_attenuation_cutoff.w) :
		0.0f;
}

float lightGetDiffuse(const vec3 normal_wcs, const vec3 vertex_to_light_wcs)
{
	return max(0.0, dot(normal_wcs, vertex_to_light_wcs));
}

float lightGetSpecular(const vec3 normal_wcs, const vec3 vertex_to_light_wcs)
{
	vec3	vertex_to_camera_wcs = normalize(uniform_camera.position_wcs - fs_in.position_wcs_v);

#ifdef BLINN_PHONG_SHADING
	return phong_blinn_specular	(vertex_to_light_wcs, vertex_to_camera_wcs, normal_wcs, 1.0f - uniform_material_specular_color.a);
#else
	return phong_specular		(vertex_to_light_wcs, vertex_to_camera_wcs, normal_wcs, 1.0f - uniform_material_specular_color.a);
#endif
}

void main(void)
{
	// [TEXTURES]
	vec4 diffuse_tex = vec4(1.0f);
	if (uniform_material_has_tex.x > 0)
	{
		diffuse_tex  = texture(uniform_textures.diffuse, fs_in.texcoord_v.xy);
		if (diffuse_tex.a < 1.0f || uniform_material_diffuse_color.a < 1.0f)
			discard;
	}

	vec4 specular_tex = vec4(1.0f);
	if (uniform_material_has_tex.z > 0)
		specular_tex  = texture(uniform_textures.specular, fs_in.texcoord_v.xy);

	vec4 emission_tex = vec4(1.0f);
	if (uniform_material_has_tex.w > 0)
		emission_tex  = texture(uniform_textures.emission, fs_in.texcoord_v.xy);

	//[SPOT][OR][DIRECTIONAL][LIGHT]
	bool	is_spot_light		= (uniform_light_position_wcs.w == 1.0f) ? true : false;
	vec3	light_direction_wcs = (is_spot_light) ? uniform_light_direction_wcs.xyz : -uniform_light_position_wcs.xyz;
		
	vec3	vertex_to_light_wcs = uniform_light_position_wcs.xyz - fs_in.position_wcs_v;
	float	dist2				= dot(vertex_to_light_wcs,vertex_to_light_wcs);
	float	dist_to_light		= length	(vertex_to_light_wcs);
			vertex_to_light_wcs	= normalize	(vertex_to_light_wcs);
	vec3	normal_wcs			= normalize	(fs_in.normal_wcs_v);

	// [SPOTLIGHT]
	float	attenuation_factor	= (is_spot_light) ? lightGetAttenuation(dist_to_light) : 1.0f;
	float	spot_angle_factor	= (is_spot_light) ? lightGetSpot(-vertex_to_light_wcs, light_direction_wcs) : 1.0f;

	// [DIFFUSE]
	float	diffuse_angle_factor= (!is_spot_light || spot_angle_factor > 0.0f) ?
			lightGetDiffuse(normal_wcs, vertex_to_light_wcs)		 : 0.0f;
	
	// [SPECULAR]
	float	specular_shininess	= (diffuse_angle_factor > 0.0f) ?
			lightGetSpecular(normal_wcs, vertex_to_light_wcs) : 0.0f;

	// [SHADOW MAPPING]
#ifdef SHADOW_MAPPING 
	float   cast_shadows		= uniform_light_direction_wcs.a;
	float	shadow_factor		= (cast_shadows == 1.0f) ? ((diffuse_angle_factor > 0.0f) ? lightGetShadow(diffuse_angle_factor) : 0.0f) : 1.0f;
#else
	float	shadow_factor		= 1.0f;
#endif

	attenuation_factor			= 1.0/dist2;

	// [FINAL COLORS]
	vec3	diffuse_color		= vec3(uniform_material_diffuse_color)  * vec3(diffuse_tex);
	vec3	specular_color		= vec3(uniform_material_specular_color) * vec3(specular_tex);
	vec3	emission_color		= vec3(uniform_material_emission_color) * vec3(emission_tex);

	// [FINAL COLORS]
	vec3	light_color			 = vec3(5000);			//THIS MUST BE IN REAL UNITS, e.g. WATTS uniform_light_diffuse_color;	

	vec3	ambient_color_final  = uniform_light_ambient_color;
	vec3	diffuse_color_final  = Lambert_BRDF(diffuse_color.rgb);
	vec3	specular_color_final = specular_color * specular_shininess;
	vec3	emission_color_final = emission_color;

	// [FINAL COLORS]
	vec3	direct_color_final	 = (diffuse_color_final + specular_color_final) * attenuation_factor * light_color * shadow_factor * diffuse_angle_factor * spot_angle_factor;
	vec3	lighting_color_final = ambient_color_final + direct_color_final + emission_color_final;

	out_color  = vec4(lighting_color_final, 1.0f);
}
