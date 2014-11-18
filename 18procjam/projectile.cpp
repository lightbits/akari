#include "projectile.h"
#include "world.h"

void update_bullet(Projectile &b, float dt)
{
	update_body(b.body, dt);
	b.lifetime -= dt;
	if (b.lifetime <= 0.0f)
		b.dead = true;
	vector<Enemy> &enemies = get_enemies();
	for (int i = 0; i < enemies.size(); i++)
	{
		if (collides(b.body, enemies[i].body))
		{
			hit_entity(enemies[i].entity, b.damage);
			b.dead = true;
			break;
		}
	}

	if (collides(b.body, get_player_body()))
	{
		hit_entity(get_player().entity, b.damage);
		b.dead = true;
	}

	if (is_offscreen(b.body))
		b.dead = true;
}