#include "enemy.h"
#include "world.h"

void update_enemy(Enemy &e, float dt)
{
	update_body(e.body, dt);
	update_laser(e.laser, dt);
	clamp_speed(e.entity.move_speed, e.body);
	e.entity.lifetime += dt;
	if (is_offscreen(e.body))
		e.entity.dead = true;

	e.behaviour(e);
}