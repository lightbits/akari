#version 140

in vec2 texel;

out vec4 outColor;

void main()
{
	float r = sqrt(dot(texel, texel));
    outColor = vec4(exp(-2.0 * r), texel.y, 0.5, 1.0);
}