#include "font.h"
#include "gfx.h"
#include "log.h"
#include "stb_image.h"

static Font *current = nullptr;

void Font::dispose()
{
	if (current == this)
		current = nullptr;
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

void use_font(Font &font)
{
	current = &font;
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

vec2i measure_string(const string &text)
{
	int height = current->char_height;
	int width = 0;
	int curr_width = 0;
	for (int i = 0; i < text.size(); i++)
	{
		char c = text[i];
		Glyph glyph = current->glyphs.find(c)->second;
		if (c == '\n')
		{
			height += current->char_height;
			curr_width = 0;
		}
		curr_width += glyph.width;
		if (curr_width > width)
			width = curr_width;
	}

	return vec2i(width, height);
}

void draw_string(float x0, float y0, const string &text, bool centered, float sx, float sy, vec4 color)
{
	// Nothing to draw
	if (text.size() == 0)
		return;

	SDL_assert(current != nullptr);

	if (centered)
	{
		vec2i size = measure_string(text);
		x0 -= size.x * sx / 2.0f;
		y0 -= size.y * sy / 2.0f;
	}

	x0 = round(x0);
	y0 = round(y0);

	std::vector<float> v_buffer;
	std::vector<uint32> i_buffer;

	float r = color.x;
	float g = color.y;
	float b = color.z;
	float a = color.w;

	float x = x0;
	float y = y0;
	float line_height = current->char_height * sy;
	uint32 i = 0;
	for (int j = 0; j < text.size(); j++)
	{
		char c = text[j];
		Glyph glyph = current->glyphs.find(c)->second;
		if (c == '\n')
		{
			y += line_height;
			x = x0;
			continue;
		}

		float w = float(glyph.width) * sx;
		float h = float(glyph.height) * sy;
		float quad[] = {
			// Position		// Color		// Texel
			x, y,			r, g, b, a,		glyph.u_left, glyph.v_bottom,
			x + w, y,		r, g, b, a,		glyph.u_right, glyph.v_bottom,
			x + w, y + h,	r, g, b, a,		glyph.u_right, glyph.v_top,
			x, y + h,		r, g, b, a,		glyph.u_left, glyph.v_top
		};
		uint32 indices[] = { i, i + 1, i + 2, i + 2, i + 3, i };

		v_buffer.insert(v_buffer.end(), quad, quad + 32);
		i_buffer.insert(i_buffer.end(), indices, indices + 6);

		x += w;
		i += 4;
	}

	uint32 index_count = i_buffer.size();

	GLuint vbo = gen_buffer(GL_ARRAY_BUFFER, v_buffer.size() * sizeof(float), &v_buffer[0]);
	GLuint ibo = gen_buffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer.size() * sizeof(float), &i_buffer[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	attribfv("position", 2, 8, 0);
	attribfv("color", 4, 8, 2);
	attribfv("texel", 2, 8, 6);
	uniform("tex", 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, current->texture);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unset_attrib("position");
	unset_attrib("color");
	unset_attrib("texel");

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}