#version 150

in vec3 position;
in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 vPosition;
out vec3 vNormal;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    vPosition = (model * vec4(position, 1.0)).xyz;
    vNormal = (model * vec4(normal, 0.0)).xyz;
}