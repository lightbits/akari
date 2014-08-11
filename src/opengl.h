#ifndef UTIL_H
#define UTIL_H
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "types.h"

uint gen_buffer(GLenum target, int size, const void *data, GLenum usage = GL_STATIC_DRAW);
bool check_gl_errors();

#endif