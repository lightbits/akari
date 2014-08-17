#ifndef FONT_H
#define FONT_H
#include "types.h"
#include "opengl.h"
#include "shader.h"

struct Glyph
{
	float u_left, u_right;
	float v_bottom, v_top;
	int width, height;
};

struct Font
{
	unordered_map<char, Glyph> glyphs;
	uint texture;
	int char_height;

	void dispose();
};

bool load_font(Font &font, 
			   string path, 
			   string char_set = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");

#endif