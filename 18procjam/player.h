#ifndef PLAYER_H
#define PLAYER_H
#include "kyoko.h"
#include "body.h"
#include "laser.h"
#include "entity.h"

struct Player
{
	uint id;
	Body body;
	Entity entity;
	vector<Laser> lasers;
};

void update_player(Player &p, float dt);

#endif