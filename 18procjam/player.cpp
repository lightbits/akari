#include "player.h"
#include "world.h"

void control_entity(Body &body, Entity &entity)
{
	if (is_key_down(SDLK_LEFT))
		body.velocity.x = -entity.move_speed;
	else if (is_key_down(SDLK_RIGHT))
		body.velocity.x = +entity.move_speed;
	else
		body.velocity.x = 0.0f;

	if (is_key_down(SDLK_UP))
		body.velocity.y = -entity.move_speed;
	else if (is_key_down(SDLK_DOWN))
		body.velocity.y = +entity.move_speed;
	else
		body.velocity.y = 0.0f;

	clamp_speed(entity.move_speed, body);
}


void update_player(Player &p, float dt)
{
	update_body(p.body, dt);
	for (int i = 0; i < p.lasers.size(); i++)
		update_laser(p.lasers[i], dt);
	control_entity(p.body, p.entity);

	if (is_key_down(SDLK_z))
	{
		vec2 direction = vec2(0.0f, -1.0f);
		for (int i = 0; i < p.lasers.size(); i++)
			entity_fire_laser(p.id, p.lasers[i], p.entity, p.body);
	}
}