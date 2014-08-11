#version 150 core

in vec3 position;
in vec2 texel;
in vec3 normal;

out vec3 vPosition;
out vec2 vTexel;
out vec3 vNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	vPosition = (model * vec4(position, 1.0)).xyz;
	vTexel = texel;
	vNormal = (model * vec4(normal, 0.0)).xyz;
	gl_Position = projection * view * vec4(vPosition, 1.0);
}