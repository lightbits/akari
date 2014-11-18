#ifndef ENEMY_H
#define ENEMY_H
#include "kyoko.h"
#include "entity.h"
#include "body.h"
#include "laser.h"
using std::function;

struct Enemy
{
	uint id;
	Body body;
	Entity entity;
	Laser laser;
	function<void(Enemy&)> behaviour;
};

void update_enemy(Enemy &e, float dt);

#endif