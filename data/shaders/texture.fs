#version 150 core

in vec2 vTexel;
uniform sampler2D tex;
out vec4 outColor;

void main()
{
	outColor = texture(tex, vTexel);
}