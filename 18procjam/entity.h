#ifndef ENTITY_H
#define ENTITY_H
#include "kyoko.h"

struct Entity
{
	// Parameters
	float move_speed;

	// State
	float lifetime;
	int hitpoints;
	bool dead;
};

void hit_entity(Entity &e, int damage);

#endif