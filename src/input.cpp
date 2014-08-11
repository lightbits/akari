#include "input.h"
#include "types.h"
#include "log.h"

bool is_key_pressed(char c)
{
	c = tolower(c);
	SDL_assert(isalpha(c));
	const uint8 *state = SDL_GetKeyboardState(NULL);
	return state[int(c) - int('a') + SDL_SCANCODE_A] > 0;
}

bool is_key_pressed(SDL_Keycode key)
{
	const uint8 *state = SDL_GetKeyboardState(NULL);
	return state[SDL_GetScancodeFromKey(key)] > 0;
}

vec2i get_mouse_pos()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	return vec2i(x, y);
}