#ifndef TEXTURE_H
#define TEXTURE_H
#include "types.h"
#include "opengl.h"

bool load_texture_from_file(
				uint &texture, const char *filename,
				GLenum min_filter = GL_LINEAR, 
				GLenum mag_filter = GL_LINEAR,
				GLenum wrap_s = GL_CLAMP_TO_EDGE, 
				GLenum wrap_t = GL_CLAMP_TO_EDGE,
				bool flip_y = false);
bool load_texture_from_mem(
				uint &texture, 
				const void *png_data, 
				int png_size,
				GLenum min_filter = GL_LINEAR, 
				GLenum mag_filter = GL_LINEAR,
				GLenum wrap_s = GL_CLAMP_TO_EDGE, 
				GLenum wrap_t = GL_CLAMP_TO_EDGE,
				bool flip_y = false);
uint gen_texture(
				const void *data, 
				int width, int height, 
				GLenum internal_fmt, 
				GLenum data_fmt, 
				GLenum data_type,
				GLenum min_filter = GL_LINEAR, 
				GLenum mag_filter = GL_LINEAR,
				GLenum wrap_s = GL_CLAMP_TO_EDGE, 
				GLenum wrap_t = GL_CLAMP_TO_EDGE);

#endif
