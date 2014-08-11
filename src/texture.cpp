#include "texture.h"
#include "log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static uint current_unit = 0;

bool load_texture_from_file(uint &texture, const char *filename)
{
	int width, height, channels;
	uint8 *pixels = stbi_load(filename, &width, &height, &channels, 4);

	if (pixels == NULL)
	{
		APP_LOG << "Failed to load texture: " << stbi_failure_reason() << '\n';
		return false;
	}

	// Flip y
	for (int y = 0; y < height / 2; y++)
	for (int x = 0; x < width * 4; x++)
		std::swap(pixels[y * width * 4 + x], pixels[(height - 1 - y) * width * 4 + x]);

	gen_texture(texture, pixels, width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	stbi_image_free(pixels);
	return true;
}

bool load_texture_from_mem(uint &texture, const void *png_data, int png_size, 
						   GLenum internal_fmt, GLenum data_fmt, GLenum data_type)
{
	int width, height, channels;
	uint8 *pixels = stbi_load_from_memory((const unsigned char*)png_data, png_size, &width, &height, &channels, 4);

	if (pixels == NULL)
	{
		APP_LOG << "Failed to load texture: " << stbi_failure_reason() << '\n';
		return false;
	}

	// Flip y
	for (int y = 0; y < height / 2; y++)
	for (int x = 0; x < width * 4; x++)
		std::swap(pixels[y * width * 4 + x], pixels[(height - 1 - y) * width * 4 + x]);

	gen_texture(texture, pixels, width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	stbi_image_free(pixels);
	return true;
}

void gen_texture(uint &texture, const void *data, int width, int height, 
				 GLenum internal_fmt, GLenum data_fmt, GLenum data_type)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_fmt, width, height, 0, data_fmt, data_type, data);
	tex_parameter(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void tex_parameter(GLenum min_filter, GLenum mag_filter, GLenum wrap_s, GLenum wrap_t)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
}

void use_texture_2D(uint texture, uint unit)
{
	if (current_unit != unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		current_unit = unit;
	}
	glBindTexture(GL_TEXTURE_2D, texture);
}
