#version 150

in vec2 vTexel;
uniform sampler2D texScene;
out vec4 outColor;

void main()
{
	outColor = texture(texScene, vTexel);
}