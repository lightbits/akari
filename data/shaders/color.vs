#version 150

in vec3 position;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
}