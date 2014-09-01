#ifndef COMMON_MATRIX_H
#define COMMON_MATRIX_H
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For glm::ortho and glm::perspective
#include <glm/gtc/type_ptr.hpp>	// For value_ptr(matrix)
#include <glm/gtc/quaternion.hpp>
using glm::perspective;
using glm::ortho;
using glm::round;
using glm::max;
using glm::min;
using glm::normalize;
using glm::length;

typedef glm::mat2 mat2;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

// Signed integer
typedef glm::ivec2 vec2i;
typedef glm::ivec3 vec3i;
typedef glm::ivec4 vec4i;

// Unsigned integer
typedef glm::uvec2 vec2u;
typedef glm::uvec3 vec3u;
typedef glm::uvec4 vec4u;

typedef glm::quat quat;

// Construct a quaternion from an angle and an axis
static quat quaternion(float angle, const vec3 &axis)
{
	return glm::angleAxis(angle, axis);
}

// Interpolate between two quaternions
static quat slerp(const quat &r, const quat &q, float t)
{
	return glm::slerp(r, q, t);
}

static mat4 rotateX(float rad)
{
	float co = cosf(rad); float si = sinf(rad);
	mat4 m(1.0f);
	m[1][1] = co; m[1][2] = -si; m[2][1] = si; m[2][2] = co;
	return m;
}

static mat4 rotateY(float rad)
{
	float co = cosf(rad); float si = sinf(rad);
	mat4 m(1.0f);
	m[0][0] = co; m[0][2] = si; m[2][0] = -si; m[2][2] = co;
	return m;
}

static mat4 rotateZ(float rad)
{
	float co = cosf(rad); float si = sinf(rad);
	mat4 m(1.0f);
	m[0][0] = co; m[1][0] = -si; m[0][1] = si; m[1][1] = co;
	return m;
}

static mat4 translate(float x, float y, float z)
{
	mat4 m(1.0f);
	m[3][0] = x; m[3][1] = y; m[3][2] = z; m[3][3] = 1.0f;
	return m;
}

static mat4 translate(const vec3 &v)
{
	mat4 m(1.0f);
	m[3][0] = v.x; m[3][1] = v.y; m[3][2] = v.z; m[3][3] = 1.0f;
	return m;
}

static mat4 scale(float x, float y, float z)
{
	mat4 m(1.0f);
	m[0][0] = x; m[1][1] = y; m[2][2] = z;
	return m;
}

static mat4 scale(const vec3 &v)
{
	mat4 m(1.0f);
	m[0][0] = v.x; m[1][1] = v.y; m[2][2] = v.z;
	return m;
}

static mat4 scale(float s)
{
	return scale(s, s, s);
}

#endif