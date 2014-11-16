#include "game.h"
#include <algorithm>

// Todo: Add gfx2d functionality to draw arbitrary
// polygons. Also specify transformation (rotation)

const uint	BG_COLOR				= 0x404968ff;
const int	PLAYER_BULLET_DAMAGE	= 5;
const int	PLAYER_HITPOINTS		= 10;
const float PLAYER_MOVE_SPEED		= 300.0f;
const float PLAYER_BULLET_SPEED		= 300.0f;
const float PLAYER_BULLET_LIFETIME	= 5.0f;
const float PLAYER_FIRE_TIME		= 0.2f;
const int	ENEMY_BULLET_DAMAGE		= 10;
const int	ENEMY_HITPOINTS			= 10;
const float ENEMY_BULLET_SPEED		= 300.0f;
const float ENEMY_BULLET_LIFETIME	= 5.0f;
const float ENEMY_MOVE_SPEED		= 100.0f;
const float ENEMY_FIRE_TIME			= 2.0f;

// Todo: Rename Entity to something else
// Make Entity a base class for all game objects

// Todo: load these from file
// Make different enemy types have diff params
// Archetypal enemies? copy from

// Todo: Make cooldown timers go down when firing
// charge up when not

// Todo: Fix GL_INVALID_VALUE when too much stuff to draw

// Todo: Make general weapon thing
// Todo: Move fire_time and bullet stuffs and cooldown in here
// Todo: Add laser body
// Todo: Add parent id
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

struct Body
{
	vec2 size;
	vec2 position;
	vec2 velocity;
	vec2 acceleration;
};

struct Entity
{
	// Parameters
	float move_speed;

	// State
	float lifetime;
	int hitpoints;
	bool dead;
};

struct Enemy
{
	uint id;
	Body body;
	Entity entity;
	Laser laser;
};

struct Player
{
	uint id;
	Body body;
	Entity entity;
	vector<Laser> lasers;
};

struct Bullet
{
	uint shooter_id;
	float lifetime;
	float damage;
	bool dead;
	Body body;
};

struct World
{
	uint next_id;
	vector<Bullet> bullets;
	vector<Enemy> enemies;
	Player player;
};

Font font;
int window_width;
int window_height;
uint tex;
World world;
float spawn_timer = 0.0f;
float spawn_period = 1.0f;

void new_enemy(vec2 position, vec2 velocity, vec2 acceleration)
{
	Entity entity;
	entity.move_speed = ENEMY_MOVE_SPEED;
	entity.hitpoints = ENEMY_HITPOINTS;
	entity.lifetime = 0.0f;
	entity.dead = false;

	Body body;
	body.size = vec2(32.0f);
	body.position = position;
	body.velocity = velocity;
	body.acceleration = acceleration;

	Laser laser;
	laser.fire_time = ENEMY_FIRE_TIME;
	laser.bullet_speed = ENEMY_BULLET_SPEED;
	laser.cooldown = ENEMY_FIRE_TIME;
	laser.damage = ENEMY_BULLET_DAMAGE;
	laser.bullet_lifetime = ENEMY_BULLET_LIFETIME;
	laser.attachment_point = vec2(16.0f, +38.0f);
	laser.normal = vec2(0.0f, 1.0f);
	laser.parent_id = world.next_id;

	Enemy enemy;
	enemy.body = body;
	enemy.entity = entity;
	enemy.laser = laser;
	enemy.id = world.next_id++;
	world.enemies.push_back(enemy);
}

void new_bullet(uint shooter_id, float lifetime, vec2 position, vec2 velocity, int damage, vec2 acceleration)
{
	Body body;
	body.position = position;
	body.velocity = velocity;
	body.acceleration = acceleration;
	body.size = vec2(2.0f, 8.0f);

	Bullet bullet;
	bullet.body = body;
	bullet.dead = false;
	bullet.damage = damage;
	bullet.lifetime = lifetime;
	bullet.shooter_id = shooter_id;
	world.bullets.push_back(bullet);
}

void init_player(vec2 position, vec2 velocity, vec2 acceleration)
{
	Laser laser_l, laser_r;
	laser_l.bullet_speed = PLAYER_BULLET_SPEED;
	laser_l.bullet_lifetime = PLAYER_BULLET_LIFETIME;
	laser_l.fire_time = PLAYER_FIRE_TIME;
	laser_l.damage = PLAYER_BULLET_DAMAGE;
	laser_l.cooldown = PLAYER_FIRE_TIME;
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
	world.player.entity.move_speed = PLAYER_MOVE_SPEED;
	world.player.entity.hitpoints = PLAYER_HITPOINTS;
	world.player.entity.dead = false;
	world.player.body.size = vec2(32.0f);
	world.player.body.position = position;
	world.player.body.velocity = velocity;
	world.player.body.acceleration = acceleration;
}

void update_body(Body &body, float dt)
{
	body.position += body.velocity * dt;
	body.velocity += body.acceleration * dt;
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

void hit_entity(Entity &e, float damage)
{
	e.hitpoints -= damage;
	if (e.hitpoints <= 0)
		e.dead = true;
}

void update_bullet(Bullet &b, float dt)
{
	update_body(b.body, dt);
	b.lifetime -= dt;
	if (b.lifetime <= 0.0f)
		b.dead = true;
	for (int i = 0; i < world.enemies.size(); i++)
	{
		if (collides(b.body, world.enemies[i].body))
		{
			hit_entity(world.enemies[i].entity, b.damage);
			b.dead = true;
			return;
		}
	}

	if (collides(b.body, world.player.body))
	{
		hit_entity(world.player.entity, b.damage);
		b.dead = true;
	}
}

void clamp_speed(float max_speed, Body &body)
{
	float speed = length(body.velocity);
	if (speed > max_speed)
		body.velocity = normalize(body.velocity) * max_speed;
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

void update_laser(Laser &l, float dt)
{
	if (l.cooldown > 0.0f)
		l.cooldown -= dt;
}

void update_enemy(Enemy &e, float dt)
{
	update_body(e.body, dt);
	update_laser(e.laser, dt);
	vec2 direction = normalize(world.player.body.position - e.body.position);
	if (entity_fire_laser(e.id, e.laser, e.entity, e.body))
	{
		// something?
		// recoil maybe
	}
	clamp_speed(e.entity.move_speed, e.body);
	e.entity.lifetime += dt;
}

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

void render_bullet(Bullet &b)
{
	using namespace gfx2d;
	draw_fill_rectangle(b.body.position, b.body.size, 0xd3baffff);
}

bool load_game(int width, int height)
{
	if (!load_font(font, "../data/fonts/proggytinyttsz_8x12.png"))
		return false;

	window_width = width;
	window_height = height;
	gfx2d::init(width, height);
	gfx2d::use_font(font);

	return true;
}

void free_game()
{
	
}

void init_game()
{
	new_enemy(vec2(200.0f, 200.0f), vec2(0.0f, 0.0f), vec2(0.0f, 0.0f));
	new_enemy(vec2(90.0f, 30.0f), vec2(50.0f, 10.0f), vec2(0.0f, -10.0f));
	new_enemy(vec2(140.0f, 30.0f), vec2(50.0f, 10.0f), vec2(0.0f));
	new_enemy(vec2(180.0f, 30.0f), vec2(50.0f, 10.0f), vec2(0.0f));
	new_enemy(vec2(220.0f, 30.0f), vec2(50.0f, 10.0f), vec2(0.0f));
	init_player(vec2(200.0f, 400.0f), vec2(0.0f), vec2(0.0f));
}

void update_game(float dt)
{
	spawn_timer -= dt;
	if (spawn_timer <= 0.0f)
	{
		new_enemy(
			vec2(
				frand() * window_width, 
				frand() * window_height * 0.35f
				), 
			vec2(
				(-1.0f + 2.0f * frand()) * 100.0f, 
				frand() * 100.0f
				), 
			vec2(
				(-1.0f + 2.0f * frand()) * 100.0f,
				(-1.0f + 2.0f * frand()) * 100.0f
			)
		);
		spawn_timer += spawn_period;
	}

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

void render_game(float dt)
{
	gfx2d::begin();
	{
		using namespace gfx2d;
		clearc(BG_COLOR);
		blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (int i = 0; i < world.enemies.size(); i++)
			render_enemy(world.enemies[i]);
		for (int i = 0; i < world.bullets.size(); i++)
			render_bullet(world.bullets[i]);
		render_player(world.player);

		Text text;
		text << "Enemies: " << world.enemies.size() << '\n';
		text << "Bullets: " << world.bullets.size() << '\n';
		draw_string(5.0f, 5.0f, text.getString());
	}
	gfx2d::end();
}