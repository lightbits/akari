#version 150 core

in vec3 position;
in vec2 texel;

out vec3 vPosition;
out vec2 vTexel;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	vPosition = (model * vec4(position, 1.0)).xyz;
	vTexel = texel;
	gl_Position = projection * view * vec4(vPosition, 1.0);
}