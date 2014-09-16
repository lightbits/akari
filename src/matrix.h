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
using glm::cross;

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
quat quaternion(float angle, const vec3 &axis);

// Interpolate between two quaternions
quat slerp(const quat &r, const quat &q, float t);

mat4 rotateX(float rad);
mat4 rotateY(float rad);
mat4 rotateZ(float rad);

mat4 translate(float x, float y, float z);
mat4 translate(const vec3 &v);

mat4 scale(float x, float y, float z);
mat4 scale(const vec3 &v);
mat4 scale(float s);

#endif