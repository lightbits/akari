#version 150

in vec4 vPosition;
out float outDepth;

void main()
{
	float zNDC = vPosition.z / vPosition.w;
	outDepth = 0.5 + zNDC * 0.5;
}