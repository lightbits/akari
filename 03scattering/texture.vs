#version 150

in vec2 position;
in vec2 texel;

out vec2 vTexel;

void main()
{
	gl_Position = vec4(position, 0.0, 1.0);
	vTexel = texel;
}