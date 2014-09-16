#include "gfx.h"

Shader current;

void clearc(uint32 color) { clearc(to_rgba(color)); }
void clearc(vec4 color)
{
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT);
}

void cleard(float depth)
{
	glClearDepth(depth);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void clear(uint32 color, float depth) { clear(to_rgba(color), depth); }
void clear(vec4 color, float depth)
{
	glClearDepth(depth);
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void cull(bool enabled, GLenum front, GLenum mode)
{
	if (enabled)
	{
		glEnable(GL_CULL_FACE);
		glFrontFace(front);
		glCullFace(mode);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
}

void depth_test(bool enabled, GLenum func)
{
	if(enabled)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(func);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}

void depth_write(bool enabled)
{
	if(enabled)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthRange(0.0, 1.0);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
	}
}

void blend_mode(bool enabled, GLenum src, GLenum dest, GLenum func)
{
	if(enabled)
	{
		glEnable(GL_BLEND);
		glBlendFunc(src, dest);
		glBlendEquation(func);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

vec4 to_rgba(uint32 hex)
{
	return vec4(
		((hex>>24) & 0xff) / 255.0f,
		((hex>>16) & 0xff) / 255.0f,
		((hex>>8) & 0xff) / 255.0f,
		(hex & 0xff) / 255.0f);
}

uint32 to_hex(vec4 rgba)
{
	rgba *= 255.0f;
	return 
		uint8(rgba.x) << 24 |
		uint8(rgba.y) << 16 |
		uint8(rgba.z) << 8  |
		uint8(rgba.w);
}

float to_rad(float degrees)
{
	return degrees * 3.14159265359f / 180.0f;
}

void use_shader(Shader shader)
{
	current = shader;
	current.use();
}

struct Attribfv
{
	string name;
	int size;
	int offset;
};
vector<Attribfv> attrib_stack;
int attrib_stack_offset = 0;
void attribfv_append(string name, int num_components)
{
	Attribfv a;
	a.name = name;
	a.size = num_components;
	a.offset = attrib_stack_offset;
	attrib_stack_offset += num_components;
	attrib_stack.push_back(a);
}

void attribfv_enable()
{
	for (int i = 0; i < attrib_stack.size(); i++)
	{
		Attribfv a = attrib_stack[i];
		attribfv(a.name, a.size, attrib_stack_offset, a.offset);
	}
	attrib_stack.clear();
	attrib_stack_offset = 0;
}

void attribfv(string name, int num_components, int stride, int offset)
{ 
	current.set_attribfv(name, num_components, stride, offset); 
}

void attribv(string name, GLenum type, int num_components, int byte_stride, int byte_offset, bool normalized)
{
	current.set_attribv(name, type, num_components, byte_stride, byte_offset, normalized);
}

void unset_attrib(string name)
{
	current.unset_attrib(name);
}

void uniform(string name, const mat4 &v) { current.set_uniform(name, v); }
void uniform(string name, const mat3 &v) { current.set_uniform(name, v); }
void uniform(string name, const mat2 &v) { current.set_uniform(name, v); }
void uniform(string name, const vec4 &v) { current.set_uniform(name, v); }
void uniform(string name, const vec3 &v) { current.set_uniform(name, v); }
void uniform(string name, const vec2 &v) { current.set_uniform(name, v); }
void uniform(string name, double v) { current.set_uniform(name, v); }
void uniform(string name, float v) { current.set_uniform(name, v); }
void uniform(string name, int v) { current.set_uniform(name, v); }
void uniform(string name, uint v) { current.set_uniform(name, v); }