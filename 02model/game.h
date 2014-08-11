#ifndef GAME_H
#define GAME_H
#include "kyoko.h"

bool load_game(int width, int height);
void free_game();
void init_game();
void update_game(float dt);
void render_game(float dt);
void on_key_up(uint16 mod, SDL_Keycode key, float dt);
void on_key_down(uint16 mod, SDL_Keycode key, float dt);
void on_mouse_moved(int x, int y, int dx, int dy, float dt);
void on_mouse_dragged(int button, int x, int y, int dx, int dy, float dt);
void on_mouse_pressed(int button, int x, int y, float dt);
void on_mouse_released(int button, int x, int y, float dt);

#endif