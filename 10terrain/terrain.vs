#version 140

in vec3 position;
in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform sampler2D heightmap;

out vec4 worldPos; // world-space vertex position
out vec3 worldNormal; // world-space normal direction
out float dist;

void main()
{
	//vec2 texel = vec2(0.5) + 0.5 * position.xz;
	//vec4 h = texture(heightmap, texel);

    worldPos = vec4(position.x, 0.0, position.z, 1.0);
	worldNormal = vec3(0.0, 1.0, 0.0);
    vec4 viewPos = view * worldPos;
    gl_Position = projection * viewPos;

    dist = length(viewPos.xyz);
}