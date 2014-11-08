#ifndef WINDOW_H
#define WINDOW_H
#include <SDL.h>

struct Window
{
	SDL_Window *sdl_handle;
	int width;
	int height;
	float frame_time;
};

#endif