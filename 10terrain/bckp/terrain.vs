#version 140

in vec3 position;
in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 worldPos; // world-space vertex position
out vec3 worldNormal; // world-space normal direction
out float dist;

void main()
{
    worldPos = model * vec4(position, 1.0);
    vec4 viewPos = view * worldPos;
    gl_Position = projection * viewPos;

    dist = length(viewPos.xyz);

    worldNormal = normalize(model * vec4(normal, 0.0)).xyz;
}