#version 150

in vec3 vPosition;
out float outDepth;

void main()
{
	outDepth = length(vPosition);
}