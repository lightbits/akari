#ifndef FONT_H
#define FONT_H
#include "types.h"
#include "opengl.h"
#include "shader.h"
// http://en.cppreference.com/w/cpp/string/basic_string/to_string INFINITELY USEFUL!


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

void use_font(Font &font);
bool load_font(Font &font, 
			   string path, 
			   string char_set = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
vec2i measure_string(const string &text);
void draw_string(float x, float y, 
				 const string &text,
				 bool centered = false,
				 float sx = 1.0f, 
				 float sy = 1.0f, 
				 vec4 color = vec4(1.0f));

#endif