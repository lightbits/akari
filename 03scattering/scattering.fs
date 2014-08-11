#version 150

in vec4 vPosition;
in vec3 vNormal;

uniform mat4 lightMatrix;
uniform vec3 lightPos;
uniform sampler2D depthMap;

out vec4 outColor;

void main()
{
	vec4 positionl = lightMatrix * vPosition;
	positionl.xyz /= positionl.w;
	vec2 texel = positionl.xy * 0.5 + vec2(0.5);
	float d_i = texture(depthMap, texel).r;
	float d_o = length(vPosition.xyz - lightPos);

	float s = abs(d_o - d_i);

	const vec3 lightColor = vec3(0.8, 0.4, 0.3);

	vec3 N = normalize(vNormal);
	vec3 L = normalize(lightPos - vPosition.xyz);
	float Id = max(dot(N, L), 0.0);
	float Is = exp(-s * 0.5);

	outColor.rgb = (Id + Is) * lightColor;
	outColor.a = 1.0;
}