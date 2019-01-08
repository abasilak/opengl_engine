#version 330 core

in vec2 TexCoords;

uniform sampler2D	uniform_texture_color;

layout(location = 0) out vec4 out_color_normal;
 
void main()
{ 
	vec3	  normal = texture(uniform_texture_color, TexCoords).rgb;
    out_color_normal = (all(equal(normal, vec3(0.0f)))) ? vec4(0.0f, 0.0f, 0.0f, 1.0f) : vec4(normal*0.5f + 0.5f, 1.0f);
}