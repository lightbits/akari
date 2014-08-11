#include "opengl.h"
#include "fileio.h"
#include "log.h"

uint gen_buffer(GLenum target, int size, const void *data, GLenum usage)
{
	uint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(target, buffer);
	glBufferData(target, size, data, usage);
	glBindBuffer(target, 0);
	return buffer;
}

const char *get_gl_error_msg(GLenum code)
{
	switch(code)
	{
	case 0: return "NO_ERROR";
	case 0x0500: return "INVALID_ENUM";
	case 0x0501: return "INVALID_VALUE";
	case 0x0502: return "INVALID_OPERATION";
	case 0x0503: return "STACK_OVERFLOW";
	case 0x0504: return "STACK_UNDERFLOW";
	case 0x0505: return "OUT_OF_MEMORY";
	case 0x0506: return "INVALID_FRAMEBUFFER_OPERATION";
	default: return "UNKNOWN";
	}
}

bool check_gl_errors()
{
	bool were_errors = false;
	GLenum error = glGetError();
	while(error != GL_NO_ERROR)
	{
		APP_LOG << "An OpenGL error occurred: " << get_gl_error_msg(error) << '\n';
		were_errors = true;
		error = glGetError();
	}
	return were_errors;
}