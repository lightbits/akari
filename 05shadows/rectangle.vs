#version 150

in vec2 position;
in vec4 color;

out vec4 vColor;

uniform mat4 projection;

void main()
{
	vColor = color;
	gl_Position = projection * vec4(position, 0.0, 1.0);
}