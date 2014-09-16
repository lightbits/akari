#version 140

in vec3 position;
in vec3 normal;

uniform mat4 view;
uniform mat4 projection;
uniform sampler2D heightmap;
uniform vec2 sampleRes;

out vec4 worldPos; // world-space vertex position
out vec3 worldNormal; // world-space normal direction
out float dist;

void main()
{
	float dx = 1.0 / (sampleRes.x - 1.0);
	float dz = 1.0 / (sampleRes.y - 1.0);

	// Scale up to world space
	vec2 texel = vec2(0.5) + 0.5 * position.xz;
	vec4 hl = texture(heightmap, texel - vec2(dx, 0.0));
	vec4 hr = texture(heightmap, texel + vec2(dx, 0.0));
	vec4 hb = texture(heightmap, texel - vec2(0.0, dz));
	vec4 ht = texture(heightmap, texel + vec2(0.0, dz));
	vec4 hc = texture(heightmap, texel);

	float height = 0.5;
	dx = 2.0 * dx;
	dz = 2.0 * dz;
	vec3 db = vec3(0.0, height * (hc.r - hb.r), -dz);
	vec3 dr = vec3(+dx, height * (hc.r - hr.r), 0.0);
	vec3 dt = vec3(0.0, height * (hc.r - ht.r), +dz);
	vec3 dl = vec3(-dx, height * (hc.r - hl.r), 0.0);

	vec3 n1 = normalize(cross(db, dr));
	vec3 n2 = normalize(cross(dr, dt));
	vec3 n3 = normalize(cross(dt, dl));
	vec3 n4 = normalize(cross(dl, db));

	worldNormal = -0.25 * (n1 + n2 + n3 + n4);

    worldPos = vec4(position.x, height * hc.r, position.z, 1.0);
    vec4 viewPos = view * worldPos;
    gl_Position = projection * viewPos;

    dist = length(viewPos.xyz);
}