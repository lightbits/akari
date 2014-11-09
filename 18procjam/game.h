#ifndef GAME_H
#define GAME_H
#include "kyoko.h"

extern bool GAME_RUNNING;

bool load_game(int width, int height);
void free_game();
void init_game();
void update_game(float dt);
void render_game(float dt);

#endif