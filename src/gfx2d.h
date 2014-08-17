#ifndef GFX_2D_H
#define GFX_2D_H
#include "types.h"
#include "font.h"

namespace gfx2d
{
	void init(int width, int height);
	void dispose();
	void begin();
	void end();
	void use_font(Font &font);
	void line(vec2 p0, vec2 p1, uint color);
	void line(float x0, float y0, float x1, float y1, uint color);
	void rectangle(vec2 p, vec2 size, uint color);
	void rectangle(float x, float y, float w, float h, uint color);
	void fill_rectangle(vec2 p, vec2 size, uint color);
	void fill_rectangle(float x, float y, float w, float h, uint color);
	void tex_rectangle(vec2 p, vec2 size, uint texture, uint color = 0xffffffff);
	void tex_rectangle(float x, float y, float w, float h, uint texture, uint color = 0xffffffff);
	void draw_string(vec2 p, const string &text, uint color = 0xffffffff, bool centered = false, float sx = 1.0f, float sy = 1.0f);
	void draw_string(float x, float y, const string &text, uint color = 0xffffffff, bool centered = false, float sx = 1.0f, float sy = 1.0f);
	vec2i measure_string(const string &text);
}

#endif