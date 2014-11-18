#include "entity.h"

void hit_entity(Entity &e, int damage)
{
	e.hitpoints -= damage;
	if (e.hitpoints <= 0)
		e.dead = true;
}