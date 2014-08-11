#ifndef INPUT_H
#define INPUT_H
#include <SDL.h>
#include "matrix.h"

bool is_key_pressed(char c);
bool is_key_pressed(SDL_Keycode key);
vec2i get_mouse_pos();

#endif