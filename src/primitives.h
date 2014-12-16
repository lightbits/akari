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
	// Generates a cube with dimensions [-0.5, 0.5] on x, y and z axes.
	// Attributes: x y z
	Mesh cube();

	// Generates a cube with dimensions [-0.5, 0.5] on x, y and z axes.
	// Attributes: x y z nx ny nz
	Mesh normal_cube();

	// Generates a grid in the xz-plane with dimension [-1.0, 1.0] on x and z axes.
	// Attributes: x y z
	Mesh grid(int lines);

	// Generates a line from v0 to v1.
	// Attributes: x y z
	Mesh line(vec3 v0, vec3 v1);

	// Generates a 2D quad with dimensions [-1.0, 1.0] on x and y axes.
	// Attributes: x y
	Mesh quad();

	// Generates a 2D quad with dimensions [-1.0, 1.0] on x and y axes.
	// Attributes: x y u v
	Mesh tex_quad();
}

#endif