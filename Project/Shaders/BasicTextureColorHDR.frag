#version 330 core

in vec2 TexCoords;

uniform sampler2D	uniform_texture_color;

const float exposure = 1.0f;

layout(location = 0) out vec4 out_color;

void main()
{ 
	vec3 hdrColor = texture(uniform_texture_color, TexCoords).rgb;

    // A. Reinhard tone mapping
    //vec3 mapped = hdrColor / (hdrColor + vec3(1.0f));

    // B. Exposure tone mapping
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

    out_color   = vec4(mapped, 1.0);
}
