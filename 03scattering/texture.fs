#version 150

in vec2 vTexel;

uniform sampler2D tex;

out vec4 outColor;

void main()
{
	float depth = texture(tex, vTexel).r;
	outColor = vec4(depth / 3.0);
}