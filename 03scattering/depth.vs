#version 150

in vec3 position;
in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 vPosition;

void main()
{
	vPosition = (view * model * vec4(position + 0.00001 * normal, 1.0)).xyz;
	gl_Position = projection * vec4(vPosition, 1.0);
}