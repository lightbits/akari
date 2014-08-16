#include "font.h"
#include "gfx.h"
#include "log.h"
#include "stb_image.h"

void Font::dispose()
{
	glDeleteTextures(1, &texture);
	glyphs.clear();
	char_height = 0;
}

typedef unordered_map<char, Glyph> GlyphMap;
bool is_border_pixel(const uint8 *pixels, uint32 x, uint32 y, uint32 width)
{
	uint32 offset = (y * width + x) * 4;
	return pixels[offset] == 0 && 
		pixels[offset + 1] == 0 && 
		pixels[offset + 2] == 0 && 
		pixels[offset + 3] == 255;
}

bool load_font(Font &font, string path, string char_set)
{
	int width, height, channels;
	uint8 *pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
	if (pixels == NULL)
	{
		APP_LOG << "Failed to load font: " << stbi_failure_reason() << '\n';
		return false;
	}

	// The pixels are now in pixels vector, 4 bytes per pixel, ordered RGBARGBA...
	// Row-wise, top-left to bottom-right

	int num_chars = char_set.size();

	font.char_height = 0;
	font.texture = 0;
	font.glyphs.clear();
	
	int index = 0;
	for (int y = 0; y < height; ++y)
	{
		int skip_y = 0;
		for (int x = 0; x < width; ++x)
		{
			if (is_border_pixel(pixels, x, y, width) && index < num_chars)
			{
				// Determine width of the glyph
				int w = 1;
				while (x + w < width && is_border_pixel(pixels, x + w, y, width))
					++w;

				// Determine height of the glyph
				int h = 1;
				while (y + h < height && is_border_pixel(pixels, x, y + h, width))
					++h;

				Glyph glyph;
				glyph.width = w - 2;
				glyph.height = h - 2;
				glyph.u_left = (x + 1) / float(width);
				glyph.u_right = (x + w - 1) / float(width);
				glyph.v_bottom = (y + 1) / float(height);
				glyph.v_top = (y + h - 1) / float(height);

				font.glyphs[char_set[index++]] = glyph;

				x += w;
				if (h > skip_y)
					skip_y = h;

				if (h > font.char_height)
					font.char_height = h;
			}
		}

		// Skip pixels (downwards) if we found any in glyphs the last horizontal-scan
		y += skip_y;
	}

	glGenTextures(1, &font.texture);
	glBindTexture(GL_TEXTURE_2D, font.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(pixels);

	return true;
}