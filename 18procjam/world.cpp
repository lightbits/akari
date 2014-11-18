#include "world.h"

struct World
{
	uint next_id;
	vector<Projectile> bullets;
	vector<Enemy> enemies;
	Player player;

	int window_width;
	int window_height;
};

static World world;

void init_world(int width, int height)
{
	world.window_width = width;
	world.window_height = height;
	world.next_id = 0;
}

void new_player(vec2 position, 
				vec2 velocity, 
				vec2 acceleration,
				float bullet_speed,
				float bullet_lifetime,
				int bullet_damage,
				float fire_time,
				float move_speed,
				int hitpoints)
{
	Laser laser_l, laser_r;
	laser_l.bullet_speed = bullet_speed;
	laser_l.bullet_lifetime = bullet_lifetime;
	laser_l.fire_time = fire_time;
	laser_l.damage = bullet_damage;
	laser_l.cooldown = fire_time;
	laser_l.attachment_point = vec2(0.0f, -8.0f);
	laser_l.normal = vec2(0.0f, -1.0f);
	laser_l.parent_id = world.next_id;

	laser_r = laser_l;
	laser_r.attachment_point = vec2(32.0f, -8.0f);

	vector<Laser> lasers;
	lasers.push_back(laser_l);
	lasers.push_back(laser_r);

	world.player.id = world.next_id++;
	world.player.lasers = lasers;
	world.player.entity.move_speed = move_speed;
	world.player.entity.hitpoints = hitpoints;
	world.player.entity.dead = false;
	world.player.body.size = vec2(32.0f);
	world.player.body.position = position;
	world.player.body.velocity = velocity;
	world.player.body.acceleration = acceleration;
}

void new_enemy(vec2 position, vec2 velocity, vec2 acceleration, function<void(Enemy&)> behaviour,
			   float move_speed, int hitpoints, float fire_time, float bullet_speed, int bullet_damage,
			   float bullet_lifetime)
{
	Entity entity;
	entity.move_speed = move_speed;
	entity.hitpoints = hitpoints;
	entity.lifetime = 0.0f;
	entity.dead = false;

	Body body;
	body.size = vec2(32.0f);

	// These should be handled by the behaviour routine
	body.position = position;
	body.velocity = velocity;
	body.acceleration = acceleration;

	Laser laser;
	laser.fire_time = fire_time;
	laser.bullet_speed = bullet_speed;
	laser.cooldown = fire_time;
	laser.damage = bullet_damage;
	laser.bullet_lifetime = bullet_lifetime;
	laser.attachment_point = vec2(16.0f, +38.0f);
	laser.normal = vec2(0.0f, 1.0f);
	laser.parent_id = world.next_id;

	Enemy enemy;
	enemy.body = body;
	enemy.entity = entity;
	enemy.laser = laser;
	enemy.id = world.next_id++;
	enemy.behaviour = behaviour;
	world.enemies.push_back(enemy);
}

void new_bullet(uint shooter_id, float lifetime, vec2 position, vec2 velocity, int damage, vec2 acceleration)
{
	Body body;
	body.position = position;
	body.velocity = velocity;
	body.acceleration = acceleration;
	body.size = vec2(2.0f, 8.0f);

	Projectile bullet;
	bullet.body = body;
	bullet.dead = false;
	bullet.damage = damage;
	bullet.lifetime = lifetime;
	bullet.shooter_id = shooter_id;

	world.bullets.push_back(bullet);
}

bool entity_fire_laser(uint id, Laser &laser, Entity &entity, Body &body)
{
	if (laser.cooldown <= 0.0f)
	{
		// Todo: rotations and stuff
		vec2 spawn = body.position + laser.attachment_point;
		vec2 direction = laser.normal;
		new_bullet(id, laser.bullet_lifetime, spawn, direction * laser.bullet_speed, laser.damage, vec2(0.0f));
		laser.cooldown += laser.fire_time;
		return true;
	}
	return false;
}

vector<Enemy> &get_enemies()
{
	return world.enemies;
}

vector<Projectile> &get_bullets()
{
	return world.bullets;
}

Player &get_player()
{
	return world.player;
}

vec2 get_player_position()
{
	return world.player.body.position;
}

Body get_player_body()
{
	return world.player.body;
}

bool collides(Body &a, Body &b)
{
	if (a.position.x > b.position.x + b.size.x ||
		a.position.x + a.size.x < b.position.x ||
		a.position.y > b.position.y + b.size.y ||
		a.position.y + a.size.y < b.position.y)
		return false;
	return true;
}

bool is_offscreen(Body &b)
{
	return (b.position.x + b.size.x < 0.0f ||
		b.position.x > world.window_width ||
		b.position.y + b.size.y < 0.0f ||
		b.position.y > world.window_height);
}

void update_world(float dt)
{
	for (int i = 0; i < world.enemies.size(); i++)
	{
		if (world.enemies[i].entity.dead)
		{
			world.enemies.erase(world.enemies.begin() + i);
			i--;
		}
		else
		{
			update_enemy(world.enemies[i], dt);
		}
	}

	for (int i = 0; i < world.bullets.size(); i++)
	{
		if (world.bullets[i].dead)
		{
			world.bullets.erase(world.bullets.begin() + i);
			i--;
		}
		else
		{
			update_bullet(world.bullets[i], dt);
		}
	}

	update_player(world.player, dt);
}

void render_enemy(Enemy &e)
{
	using namespace gfx2d;
	draw_rectangle(e.body.position, e.body.size, 0xff9944ff);

	Text text;
	text << int(e.entity.hitpoints);
	draw_string(e.body.position, text.getString());
}

void render_player(Player &p)
{
	using namespace gfx2d;
	draw_rectangle(p.body.position, p.body.size, 0x4499ffff);

	Text text;
	text << int(p.entity.hitpoints);
	draw_string(p.body.position, text.getString());
}

void render_bullet(Projectile &b)
{
	using namespace gfx2d;
	draw_fill_rectangle(b.body.position, b.body.size, 0xd3baffff);
}

void render_world(float dt)
{
	for (int i = 0; i < world.enemies.size(); i++)
		render_enemy(world.enemies[i]);
	for (int i = 0; i < world.bullets.size(); i++)
		render_bullet(world.bullets[i]);
	render_player(world.player);
}