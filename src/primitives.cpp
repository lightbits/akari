#include "primitives.h"
#include "gfx.h"
#include "log.h"

Mesh gen_normal_cube()
{
	const float hs = 0.5f;

	// All faces are oriented counter-clockwise outwards
	float vertices[] = {
		// Front
		-hs, -hs,  hs, 0.0f, 0.0f, 1.0f,
		 hs, -hs,  hs, 0.0f, 0.0f, 1.0f,
		 hs,  hs,  hs, 0.0f, 0.0f, 1.0f,
		-hs,  hs,  hs, 0.0f, 0.0f, 1.0f,

		// Back
		 hs, -hs, -hs, 0.0f, 0.0f, -1.0f,
		-hs, -hs, -hs, 0.0f, 0.0f, -1.0f,
		-hs,  hs, -hs, 0.0f, 0.0f, -1.0f,
		 hs,  hs, -hs, 0.0f, 0.0f, -1.0f,

		// Left
		-hs, -hs, -hs, -1.0f, 0.0f, 0.0f,
		-hs, -hs,  hs, -1.0f, 0.0f, 0.0f,
		-hs,  hs,  hs, -1.0f, 0.0f, 0.0f,
		-hs,  hs, -hs, -1.0f, 0.0f, 0.0f,

		// Right
		 hs, -hs,  hs, 1.0f, 0.0f, 0.0f,
		 hs, -hs, -hs, 1.0f, 0.0f, 0.0f,
		 hs,  hs, -hs, 1.0f, 0.0f, 0.0f,
		 hs,  hs,  hs, 1.0f, 0.0f, 0.0f,

		// Top
		-hs,  hs,  hs, 0.0f, 1.0f, 0.0f,
		 hs,  hs,  hs, 0.0f, 1.0f, 0.0f,
		 hs,  hs, -hs, 0.0f, 1.0f, 0.0f,
		-hs,  hs, -hs, 0.0f, 1.0f, 0.0f,

		// Bottom
		 hs, -hs,  hs, 0.0f, -1.0f, 0.0f,
		-hs, -hs,  hs, 0.0f, -1.0f, 0.0f,
		-hs, -hs, -hs, 0.0f, -1.0f, 0.0f,
		 hs, -hs, -hs, 0.0f, -1.0f, 0.0f
	};

	uint32 indices[] = {
		0, 1, 2, 2, 3, 0, // Front
		4, 5, 6, 6, 7, 4, // Back
		8, 9, 10, 10, 11, 8, // Left
		12, 13, 14, 14, 15, 12, // Right
		16, 17, 18, 18, 19, 16, // Top
		20, 21, 22, 22, 23, 20 // Bottom
	};

	Mesh mesh;
	mesh.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices);
	mesh.index_buffer = gen_buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices);
	mesh.num_indices = 36;
	mesh.num_vertices = 24;
	mesh.draw = [=]() {
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffer);
		attribfv("position", 3, 6, 0);
		attribfv("normal", 3, 6, 3);
		glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, 0);
	};
	return mesh;
}

Mesh gen_grid(int lines)
{
	vec3 *vertices = new vec3[lines * 4];

	float dx = 2.0f / (lines - 1);
	for (int i = 0; i < lines; ++i)
	{
		float x = -1.0f + i * dx;
		vertices[i * 4 + 0] = vec3(x, 0.0f, -1.0f);
		vertices[i * 4 + 1] = vec3(x, 0.0f, +1.0f);
		vertices[i * 4 + 2] = vec3(-1.0f, 0.0f, x);
		vertices[i * 4 + 3] = vec3(+1.0f, 0.0f, x);
	}

	Mesh mesh;
	mesh.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, lines * 4 * 3 * sizeof(float), vertices);
	mesh.num_indices = 0;
	mesh.num_vertices = lines * 4;
	mesh.draw = [=]() {
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
		attribfv("position", 3, 3, 0);
		glDrawArrays(GL_LINES, 0, mesh.num_vertices);
	};

	delete[] vertices;
	return mesh;
}

Mesh gen_quad()
{
	const float hs = 1.0f;

	// All faces are oriented counter-clockwise outwards
	float vertices[] = {
		-hs, -hs,
		 hs, -hs,
		 hs,  hs,
		 hs,  hs,
		-hs,  hs,
		-hs, -hs
	};

	Mesh mesh;
	mesh.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices);
	mesh.num_indices = 0;
	mesh.num_vertices = 6;
	mesh.draw = [=]() {
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffer);
		attribfv("position", 2, 2, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	};
	return mesh;
}

Mesh gen_tex_quad()
{
	const float hs = 1.0f;

	// All faces are oriented counter-clockwise outwards
	float vertices[] = {
		-hs, -hs, 0.0f, 0.0f,
		 hs, -hs, 1.0f, 0.0f,
		 hs,  hs, 1.0f, 1.0f,
		-hs,  hs, 0.0f, 1.0f
	};

	uint8 indices[] = { 0, 1, 2, 2, 3, 0 };

	Mesh mesh;
	mesh.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices);
	mesh.index_buffer = gen_buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices);
	mesh.num_indices = 6;
	mesh.num_vertices = 4;
	mesh.draw = [=]() {
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffer);
		attribfv("position", 2, 4, 0);
		attribfv("texel", 2, 4, 2);
		glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_BYTE, 0);
	};
	return mesh;
}

Mesh gen_line(vec3 v0, vec3 v1)
{
	vec3 vertices[] = { v0, v1 };

	Mesh mesh;
	mesh.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices);
	mesh.num_indices = 0;
	mesh.num_vertices = 2;
	mesh.draw = [=]() {
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
		attribfv("position", 3, 3, 0);
		glDrawArrays(GL_LINES, 0, mesh.num_vertices);
	};

	return mesh;
}

Model::Model() : mat(1.0f)
{

}

Model::Model(Mesh mesh)
{
	this->mesh = mesh;
	this->mat = mat4(1.0f);
}

void Model::draw()
{
	uniform("model", mat);
	mesh.draw();
}

void Model::dispose()
{
	mesh.dispose();
}

Mesh::Mesh()
{
	vertex_buffer = 0;
	index_buffer = 0;
	num_indices = 0;
	num_vertices = 0;
	draw = []() { APP_LOG << "[Warning] Mesh not initialized\n"; };
}

void Mesh::dispose()
{
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &index_buffer);
}