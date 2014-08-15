#ifndef GFX_2D_H
#define GFX_2D_H
#include "types.h"

namespace gfx2d
{
	void init(int width, int height);
	void begin();
	void end();
	void line(float x0, float y0, float x1, float y1, uint color);
	void rectangle(float x, float y, float w, float h, uint color);
	void fill_rectangle(float x, float y, float w, float h, uint color);
}

#endif