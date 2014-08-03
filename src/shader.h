#ifndef SHADER_H
#define SHADER_H
#include "opengl.h"
#include "matrix.h"
#include "types.h"
#include <unordered_map>

class Shader
{
public:
	bool load_from_src(string *sources, GLenum *types, int count);
	bool load_from_src(string vs_src, string fs_src);
	bool load_from_file(string *paths, GLenum *types, int count);
	bool load_from_file(string vs_path, string fs_path);
	void dispose();

	void use();
	void unuse();

	int get_uniform_location(string name);
	int get_attribute_location(string name);

	void set_attribfv(string name, int num_components, int stride, int offset);
	void unset_attrib(string name);

	void set_uniform(string name, const mat4 &v);
	void set_uniform(string name, const mat3 &v);
	void set_uniform(string name, const mat2 &v);
	void set_uniform(string name, const vec4 &v);
	void set_uniform(string name, const vec3 &v);
	void set_uniform(string name, const vec2 &v);
	void set_uniform(string name, double v);
	void set_uniform(string name, float v);
	void set_uniform(string name, int v);
	void set_uniform(string name, uint v);
private:
	uint m_id;
	std::unordered_map<string, int> m_attributes;
	std::unordered_map<string, int> m_uniforms;
};

#endif