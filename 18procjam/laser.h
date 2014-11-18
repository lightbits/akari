#ifndef LASER_H
#define LASER_H
#include "kyoko.h"

struct Laser
{
	uint parent_id;
	vec2 attachment_point;
	vec2 normal;
	float fire_time;
	float cooldown;
	float bullet_lifetime;
	float bullet_speed; // laser != bullet ...
	int damage;
};

void update_laser(Laser &l, float dt);

#endif