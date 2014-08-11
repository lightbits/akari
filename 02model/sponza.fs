#version 150 core

in vec3 vPosition;
in vec2 vTexel;

out vec4 outColor;

float texelLine(float u)
{
	float t = 0.01;
	return smoothstep(0.25 - t, 0.25, u) + smoothstep(t, 0.0, u);
}

void main()
{
	float u = texelLine(mod(vTexel.x, 0.25));
	float v = texelLine(mod(vTexel.y, 0.25));
	float s = u + v - u * v;
	outColor.rgb = vec3(1.0 - s);
	outColor.rgb = sqrt(outColor.rgb);
	outColor.rgb *= exp(-length(vPosition) * 0.1);
	outColor.a = 1.0;
}