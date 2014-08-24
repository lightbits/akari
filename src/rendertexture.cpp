#include "rendertexture.h"

uint gen_renderbuffer(int width, int height, GLenum format)
{
	uint rdb;
	glGenRenderbuffers(1, &rdb);
	glBindRenderbuffer(GL_RENDERBUFFER, rdb);
	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return rdb;
}

RenderTexture gen_rendertexture(int width, int height, GLenum internal_format)
{
	RenderTexture rt;
	rt.color = gen_texture(NULL, width, height, internal_format, GL_RGBA, GL_UNSIGNED_BYTE);
	rt.depth = gen_renderbuffer(width, height, GL_DEPTH_COMPONENT);
	rt.width = width;
	rt.height = height;

	glGenFramebuffers(1, &rt.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, rt.fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt.color, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rt.depth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return rt;
}