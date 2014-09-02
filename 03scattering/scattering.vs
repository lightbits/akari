#version 150

in vec3 position;
in vec3 normal;

out vec3 vNormal;
out vec4 vPosition;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	vNormal = (model * vec4(normal, 0.0)).xyz;
	gl_Position = projection * view * model * vec4(position, 1.0);
	vPosition = model * vec4(position, 1.0);
}