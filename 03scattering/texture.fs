#version 150

in vec2 vTexel;

uniform sampler2D texMinDepth;
uniform sampler2D texMaxDepth;
uniform float zNear;
uniform float zFar;

out vec4 outColor;

void main()
{
	float thickness = (texture(texMaxDepth, vTexel).r - texture(texMinDepth, vTexel).r) * (zFar - zNear);
	outColor = vec4(thickness * 4.0);
	// float depth = texture(tex, vTexel).r;
	// outColor = vec4(depth / 3.0);
}