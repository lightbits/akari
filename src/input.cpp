#include "input.h"
#include "types.h"
#include "log.h"

bool is_key_down(char c)
{
	c = tolower(c);
	SDL_assert(isalpha(c));
	const uint8 *state = SDL_GetKeyboardState(NULL);
	return state[int(c) - int('a') + SDL_SCANCODE_A] > 0;
}

bool is_key_down(SDL_Keycode key)
{
	const uint8 *state = SDL_GetKeyboardState(NULL);
	return state[SDL_GetScancodeFromKey(key)] > 0;
}

bool is_mouse_down(MouseButton button)
{
	uint32 state = SDL_GetMouseState(NULL, NULL);
	return (state & SDL_BUTTON(button)) > 0;
}

vec2i get_mouse_pos()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	return vec2i(x, y);
}