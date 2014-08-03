#ifndef UTIL_H
#define UTIL_H
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "types.h"

uint gen_buffer(GLenum target, int size, const void *data);
bool load_texture(uint &texture, 
				  const char *filename,
				  GLenum min_filter = GL_LINEAR, 
				  GLenum mag_filter = GL_LINEAR, 
				  GLenum wrap_s = GL_CLAMP_TO_EDGE, 
				  GLenum wrap_t = GL_CLAMP_TO_EDGE);
bool check_gl_errors();

#endif