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

Mesh gen_cube();
Mesh gen_grid(int lines);
Mesh gen_line(vec3 v0, vec3 v1);
Mesh gen_normal_cube();
Mesh gen_tex_quad();

#endif