#version 330 core

in vec2 TexCoords;

uniform sampler2D	text;
uniform vec3		uniform_text_color;

layout(location = 0) out vec4 out_color;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    
	out_color	 = vec4(uniform_text_color, 1.0) * sampled;
} 