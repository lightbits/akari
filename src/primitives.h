#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include "opengl.h"
#include "shader.h"
#include "types.h"
#include <functional>

struct Mesh
{
	Mesh();
	GLuint vertex_buffer;
	GLuint index_buffer;
	int num_indices;
	int num_vertices;

	std::function<void()> draw;
	void dispose();
};

struct Model
{
	Model();
	Model(Mesh mesh);
	Mesh mesh;
	mat4 mat;

	void dispose();
	void draw();
};

namespace Primitive
{
	Mesh cube();
	Mesh normal_cube();
	Mesh grid(int lines);
	Mesh line(vec3 v0, vec3 v1);
	Mesh quad();
	Mesh tex_quad();
}

#endif