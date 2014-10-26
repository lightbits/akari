#version 150

in vec3 position;
in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 vPosition;
out vec3 vNormal;
out vec3 vViewPosition;

void main()
{
    vec4 worldPos = model * vec4(position, 1.0);
    vec4 viewPos = view * worldPos;
    gl_Position = projection * viewPos;

    vViewPosition = viewPos.xyz;
    vPosition = worldPos.xyz;
    vNormal = (model * vec4(normal, 0.0)).xyz;
}