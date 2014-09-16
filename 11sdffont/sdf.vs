#version 150 core

in vec2 position;
in vec2 texel;
uniform float zoom;

out vec2 vTexel;

void main() 
{
	vTexel = 0.5 * (zoom * (2.0 * texel - 1.0)) + 0.5;
	gl_Position = vec4(position, 0.0, 1.0);
}