#ifndef INPUT_H
#define INPUT_H
#include <SDL.h>
#include "matrix.h"

enum MouseButton
{
	MouseButtonLeft = SDL_BUTTON_LEFT,
	MouseButtonRight = SDL_BUTTON_RIGHT,
	MouseButtonMiddle = SDL_BUTTON_MIDDLE
};

bool is_key_down(char c);
bool is_key_down(SDL_Keycode key);
bool is_mouse_down(MouseButton button);
vec2i get_mouse_pos();

#endif