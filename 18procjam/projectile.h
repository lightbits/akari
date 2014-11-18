#ifndef PROJECTILE_H
#define PROJECTILE_H
#include "kyoko.h"
#include "body.h"

struct Projectile
{
	uint shooter_id;
	float lifetime;
	int damage;
	bool dead;
	Body body;
};

void update_bullet(Projectile &b, float dt);

#endif