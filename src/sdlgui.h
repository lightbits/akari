#ifndef SDL_GUI_H
#define SDL_GUI_H
#include "imgui/imgui.h"
#include "SDL.h"

namespace gui
{
	void init(int window_width, int window_height);
	void dispose();
	void poll_events(const SDL_Event &event);
	void update(float dt);
}

#endif