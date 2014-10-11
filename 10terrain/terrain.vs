#version 140

in vec3 position;
in vec3 normal;

uniform mat4 view;
uniform mat4 projection;
uniform sampler2D heightmap;
uniform vec2 sampleRes;
uniform float time;

out vec4 worldPos; // world-space vertex position
out vec3 worldNormal; // world-space normal direction
out float dist;

void main()
{
	float dx = 1.0 / (sampleRes.x - 1.0);
	float dz = 1.0 / (sampleRes.y - 1.0);

	// // Scale up to world space
	// vec2 texel = vec2(0.5) + 0.5 * position.xz;
	// vec4 hl = texture(heightmap, texel - vec2(dx, 0.0));
	// vec4 hr = texture(heightmap, texel + vec2(dx, 0.0));
	// vec4 hb = texture(heightmap, texel - vec2(0.0, dz));
	// vec4 ht = texture(heightmap, texel + vec2(0.0, dz));
	// vec4 hc = texture(heightmap, texel);
	// 
	// float height = 0.5;
	// dx = 2.0 * dx;
	// dz = 2.0 * dz;
	// vec3 db = vec3(0.0, height * (hc.r - hb.r), -dz);
	// vec3 dr = vec3(+dx, height * (hc.r - hr.r), 0.0);
	// vec3 dt = vec3(0.0, height * (hc.r - ht.r), +dz);
	// vec3 dl = vec3(-dx, height * (hc.r - hl.r), 0.0);
	// 
	// vec3 n1 = normalize(cross(db, dr));
	// vec3 n2 = normalize(cross(dr, dt));
	// vec3 n3 = normalize(cross(dt, dl));
	// vec3 n4 = normalize(cross(dl, db));
	// 
	// worldNormal = -0.25 * (n1 + n2 + n3 + n4);

	float r2 = dot(position.xz, position.xz);
	float h = (0.5 + 0.5 * cos(3.14 * 8.0 * r2 - 10.0 * time) + 0.5 * sin(3.14 * 2.0 * r2 - 6.0 * time)) * (0.1 + 0.3 * exp(-3.0 * r2));
	worldNormal = vec3(0.0, 1.0, 0.0);

    worldPos = vec4(position.x, 0.4 * h, position.z, 1.0);
    vec4 viewPos = view * worldPos;
    gl_Position = projection * viewPos;

    dist = length(viewPos.xyz);
}