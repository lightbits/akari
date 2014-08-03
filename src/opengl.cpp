#include "opengl.h"
#include "fileio.h"
#include "log.h"
#include "lodepng.h"

uint gen_buffer(GLenum target, int size, const void *data)
{
	uint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(target, buffer);
	glBufferData(target, size, data, GL_STATIC_DRAW);
	glBindBuffer(target, 0);
	return buffer;
}

bool load_texture(uint &texture, 
				  const char *filename, 
				  GLenum min_filter, 
				  GLenum mag_filter, 
				  GLenum wrap_s, 
				  GLenum wrap_t)
{
	vector<uint8> pixels;
	uint width, height;
	uint error = lodepng::decode(pixels, width, height, filename);

	if (error)
	{
		APP_LOG << "Failed to load texture: " << error << ": " << lodepng_error_text(error) << '\n';
		return false;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// RGBA is the default parameter in the decode function
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
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