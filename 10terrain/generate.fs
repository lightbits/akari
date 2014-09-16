#version 140

in vec2 texel;

out vec4 outColor;

void main()
{
    outColor = vec4(texel.x, texel.y, 0.5, 1.0);
}