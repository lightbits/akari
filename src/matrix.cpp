#include "matrix.h"

quat quaternion(float angle, const vec3 &axis)
{
	return glm::angleAxis(angle, axis);
}

quat slerp(const quat &r, const quat &q, float t)
{
	return glm::slerp(r, q, t);
}

mat4 rotateX(float rad)
{
	float co = cosf(rad); float si = sinf(rad);
	mat4 m(1.0f);
	m[1][1] = co; m[1][2] = -si; m[2][1] = si; m[2][2] = co;
	return m;
}

mat4 rotateY(float rad)
{
	float co = cosf(rad); float si = sinf(rad);
	mat4 m(1.0f);
	m[0][0] = co; m[0][2] = si; m[2][0] = -si; m[2][2] = co;
	return m;
}

mat4 rotateZ(float rad)
{
	float co = cosf(rad); float si = sinf(rad);
	mat4 m(1.0f);
	m[0][0] = co; m[1][0] = -si; m[0][1] = si; m[1][1] = co;
	return m;
}

mat4 translate(float x, float y, float z)
{
	mat4 m(1.0f);
	m[3][0] = x; m[3][1] = y; m[3][2] = z; m[3][3] = 1.0f;
	return m;
}

mat4 translate(const vec3 &v)
{
	mat4 m(1.0f);
	m[3][0] = v.x; m[3][1] = v.y; m[3][2] = v.z; m[3][3] = 1.0f;
	return m;
}

mat4 scale(float x, float y, float z)
{
	mat4 m(1.0f);
	m[0][0] = x; m[1][1] = y; m[2][2] = z;
	return m;
}

mat4 scale(const vec3 &v)
{
	mat4 m(1.0f);
	m[0][0] = v.x; m[1][1] = v.y; m[2][2] = v.z;
	return m;
}

mat4 scale(float s)
{
	return scale(s, s, s);
}