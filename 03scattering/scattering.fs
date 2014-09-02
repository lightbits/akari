#version 150

in vec4 vPosition;
in vec3 vNormal;

uniform mat4 lightMatrix;
uniform vec3 lightPos;
uniform vec3 eyePos;
uniform sampler2D texMinDepth;
uniform sampler2D texMaxDepth;
uniform float zNear;
uniform float zFar;
uniform float density;
uniform float specularPower;
uniform float specularIntensity;

out vec4 outColor;

void main()
{
	vec4 positionl = lightMatrix * vPosition;
	positionl.xyz /= positionl.w;
	vec2 texel = positionl.xy * 0.5 + vec2(0.5);
	float s = (texture(texMaxDepth, texel).r - texture(texMinDepth, texel).r) * (zFar - zNear);

	float Ld = length(lightPos - vPosition.xyz);
	s = min(s, Ld);
	const vec3 lightColor = vec3(0.8, 0.4, 0.3);

	vec3 N = normalize(vNormal);
	vec3 L = normalize(lightPos - vPosition.xyz);
	vec3 V = normalize(eyePos - vPosition.xyz);
	vec3 R = reflect(L, N);
	float specular = pow(max(dot(V, R), 0.0), specularPower) * specularIntensity;
	float Id = max(dot(N, L), 0.0);
	float Is = exp(-s * density);

	outColor.rgb = (Id + Is + specular) * lightColor;
	outColor.a = 1.0;
}