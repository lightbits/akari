#version 150 core

in vec3 vPosition;
in vec2 vTexel;
in vec3 vNormal;

uniform float time;

out vec4 outColor;

float texelLine(float u, float r)
{
	float t = 0.01;
	float f = mod(u, r);
	return smoothstep(r - t, r, f) + smoothstep(t, 0.0, f);
}

void main()
{
	vec3 lightPos = vec3(sin(time), sin(time * 0.25), cos(time));
	vec3 lightCol = vec3(1.0, 0.4, 0.3);
	vec3 ambient = vec3(0.02, 0.03, 0.05);
	vec3 P = vPosition;
	vec3 L = lightPos - P;
	float R = length(L);
	L /= R;
	vec3 N = normalize(vNormal);
	float NdotL = max(dot(N, L), 0.0);
	float attenuation = NdotL * 1.0 / (1.0 + R * R);
	outColor.rgb = attenuation * lightCol + (1.0 - attenuation) * ambient;
	outColor.a = 1.0;

	float r = 0.25;
	float u = texelLine(vTexel.x, r);
	float v = texelLine(vTexel.y, r);
	float s = u + v - u * v;
	outColor.rgb *= 1.0 - s * 0.0001;
	outColor.rgb = sqrt(outColor.rgb);
}