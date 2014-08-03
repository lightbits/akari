#version 150 core

in vec2 position;
in vec4 color;
in vec2 texel;
out vec2 vTexel;
out vec4 vColor;
uniform mat4 projection;

void main() 
{
	vTexel = texel;
	vColor = color;
	gl_Position = projection * vec4(position, 0.0, 1.0);
}