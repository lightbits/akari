#ifndef GFX_H
#define GFX_H
#include "types.h"
#include "shader.h"
#include "opengl.h"
#include "texture.h"

void clearc(uint32 color);
void cleard(float depth);
void clear(uint32 color, float depth);
void cull(bool enabled, GLenum front = GL_CCW, GLenum mode = GL_BACK);
void depth_test(bool enabled, GLenum func = GL_LEQUAL);
void depth_write(bool enabled);
void blend_mode(bool enabled, GLenum src = GL_ONE, GLenum dest = GL_ZERO, GLenum func = GL_FUNC_ADD);

vec4 to_rgba(unsigned int hex);
float to_rad(float degrees);

void use_shader(Shader shader);
void attribfv_append(string name, int num_components);
void attribfv_enable();
void attribfv(string name, int num_components, int stride, int offset);
void attribv(string name, GLenum type, int num_components, int byte_stride, int byte_offset, bool normalized = false);
void unset_attrib(string name);
void uniform(string name, const mat4 &v);
void uniform(string name, const mat3 &v);
void uniform(string name, const mat2 &v);
void uniform(string name, const vec4 &v);
void uniform(string name, const vec3 &v);
void uniform(string name, const vec2 &v);
void uniform(string name, double v);
void uniform(string name, float v);
void uniform(string name, int v);
void uniform(string name, uint v);

#endif