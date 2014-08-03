#include "gfx.h"

Shader current;

void clearc(uint32 color)
{
	vec4 rgba = to_rgba(color);
	glClearColor(rgba.x, rgba.y, rgba.z, rgba.w);
	glClear(GL_COLOR_BUFFER_BIT);
}

void cleard(float depth)
{
	glClearDepth(depth);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void clear(uint32 color, float depth)
{
	vec4 rgba = to_rgba(color);
	glClearDepth(depth);
	glClearColor(rgba.x, rgba.y, rgba.z, rgba.w);
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

float to_rad(float degrees)
{
	return degrees * 3.14159265359f / 180.0f;
}

void use_shader(Shader shader)
{
	current = shader;
	current.use();
}

void attribfv(string name, GLsizei num_components, GLsizei stride, GLsizei offset)
{ 
	current.set_attribfv(name, num_components, stride, offset); 
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