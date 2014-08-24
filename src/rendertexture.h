#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H
#include "texture.h"

struct RenderTexture
{
	uint fbo;
	uint color;
	uint depth;
	int width;
	int height;
};

RenderTexture gen_rendertexture(int width, int height, GLenum internal_fmt, GLenum min_filter = GL_NEAREST, GLenum mag_filter = GL_NEAREST);

#endif