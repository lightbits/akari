#version 150

in vec3 position;
in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec4 vPosition;

void main()
{
	gl_Position = projection * view * model * vec4(position + 0.1 * normal, 1.0);
	vPosition = gl_Position;
}