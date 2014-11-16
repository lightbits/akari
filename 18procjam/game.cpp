#include "game.h"
#include <algorithm>

// Todo: Add gfx2d functionality to draw arbitrary
// polygons. Also specify transformation (rotation)

const uint	BG_COLOR				= 0x404968ff;
const int	PLAYER_BULLET_DAMAGE	= 5;
const int	PLAYER_HITPOINTS		= 10;
const float PLAYER_MOVE_SPEED		= 300.0f;
const float PLAYER_BULLET_SPEED		= 300.0f;
const float PLAYER_FIRE_TIME		= 0.2f;
const int	ENEMY_BULLET_DAMAGE		= 10;
const int	ENEMY_HITPOINTS			= 10;
const float ENEMY_BULLET_SPEED		= 300.0f;
const float ENEMY_MOVE_SPEED		= 100.0f;
const float ENEMY_FIRE_TIME			= 2.0f;

// Todo: load these from file
// Make different enemy types have diff params
// Archetypal enemies? copy from

// Todo: Make cooldown timers go down when firing
// charge up when not

struct WorldObject
{
	vec2 size;
	vec2 position;
	vec2 velocity;
	vec2 acceleration;
};

struct LivingThing
{
	// Parameters
	float fire_time;
	float move_speed;
	float bullet_speed;
	int bullet_damage;

	// State
	float cooldown;
	float lifetime;
	int hitpoints;
	bool dead;
};

struct Enemy
{
	// Parameters
	float fire_time;
	float move_speed;
	float bullet_speed;
	int bullet_damage;

	// State
	float cooldown;
	int hitpoints;
	bool dead;
	float lifetime;

	WorldObject obj;
};

struct Player
{
	// Parameters
	float move_speed;
	float fire_time;
	float bullet_speed;
	int bullet_damage;

	// State
	float cooldown;
	int hitpoints;
	bool dead;

	WorldObject obj;
};

struct Bullet
{
	float damage;
	bool dead;
	WorldObject obj;
};

struct World
{
	vector<Bullet> bullets;
	vector<Enemy> enemies;
	Player player;
};

Font font;
int window_width;
int window_height;
uint tex;
World world;

void new_enemy(vec2 position, vec2 velocity, vec2 acceleration)
{
	Enemy e;
	e.fire_time = ENEMY_FIRE_TIME;
	e.move_speed = ENEMY_MOVE_SPEED;
	e.bullet_speed = ENEMY_BULLET_SPEED;
	e.cooldown = ENEMY_FIRE_TIME;
	e.hitpoints = ENEMY_HITPOINTS;
	e.bullet_damage = ENEMY_BULLET_DAMAGE;
	e.lifetime = 0.0f;
	e.dead = false;
	e.obj.size = vec2(32.0f);
	e.obj.position = position;
	e.obj.velocity = velocity;
	e.obj.acceleration = acceleration;
	world.enemies.push_back(e);
}

void new_bullet(vec2 position, vec2 velocity, int damage, vec2 acceleration)
{
	Bullet b;
	b.obj.position = position;
	b.obj.velocity = velocity;
	b.obj.acceleration = acceleration;
	b.obj.size = vec2(2.0f, 8.0f);
	b.dead = false;
	b.damage = damage;
	world.bullets.push_back(b);
}

void init_player(vec2 position, vec2 velocity, vec2 acceleration)
{
	world.player.fire_time = PLAYER_FIRE_TIME;
	world.player.move_speed = PLAYER_MOVE_SPEED;
	world.player.bullet_speed = PLAYER_BULLET_SPEED;
	world.player.cooldown = 0.0f;
	world.player.hitpoints = PLAYER_HITPOINTS;
	world.player.bullet_damage = PLAYER_BULLET_DAMAGE;
	world.player.dead = false;
	world.player.obj.size = vec2(32.0f);
	world.player.obj.position = position;
	world.player.obj.velocity = velocity;
	world.player.obj.acceleration = acceleration;
}

void update_world_object(WorldObject &obj, float dt)
{
	obj.position += obj.velocity * dt;
	obj.velocity += obj.acceleration * dt;
}

bool collides(WorldObject &a, WorldObject &b)
{
	if (a.position.x > b.position.x + b.size.x ||
		a.position.x + a.size.x < b.position.x ||
		a.position.y > b.position.y + b.size.y ||
		a.position.y + a.size.y < b.position.y)
		return false;
	return true;
}

void hit_enemy(Enemy &e, float damage)
{
	e.hitpoints -= damage;
	if (e.hitpoints <= 0)
		e.dead = true;
}

void hit_player(Player &p, float damage)
{
	p.hitpoints -= damage;
	if (p.hitpoints <= 0)
		p.dead = true;
}

void update_bullet(Bullet &b, float dt)
{
	update_world_object(b.obj, dt);
	for (int i = 0; i < world.enemies.size(); i++)
	{
		if (collides(b.obj, world.enemies[i].obj))
		{
			hit_enemy(world.enemies[i], b.damage);
			b.dead = true;
			return;
		}
	}

	if (collides(b.obj, world.player.obj))
	{
		hit_player(world.player, b.damage);
		b.dead = true;
	}
}

void clamp_speed(float max_speed, WorldObject &obj)
{
	float speed = length(obj.velocity);
	if (speed > max_speed)
		obj.velocity = normalize(obj.velocity) * max_speed;
}

void update_enemy(Enemy &e, float dt)
{
	update_world_object(e.obj, dt);
	e.cooldown -= dt;
	if (e.cooldown <= 0.0f)
	{
		vec2 direction = normalize(world.player.obj.position - e.obj.position);
		vec2 velocity = direction * e.bullet_speed;
		float body_radius = length(e.obj.size * 0.8f);
		vec2 spawn_position = e.obj.position + direction * body_radius;
		new_bullet(spawn_position, velocity, e.bullet_damage, vec2(0.0f));
		// recoil on enemy?
		e.cooldown += e.fire_time;
	}
	clamp_speed(e.move_speed, e.obj);
	e.lifetime += dt;
}

void update_player(Player &p, float dt)
{
	if (p.cooldown > 0.0f)
		p.cooldown -= dt;

	if (is_key_down(SDLK_LEFT))
		p.obj.velocity.x = -p.move_speed;
	else if (is_key_down(SDLK_RIGHT))
		p.obj.velocity.x = +p.move_speed;
	else
		p.obj.velocity.x = 0.0f;

	if (is_key_down(SDLK_UP))
		p.obj.velocity.y = -p.move_speed;
	else if (is_key_down(SDLK_DOWN))
		p.obj.velocity.y = +p.move_speed;
	else
		p.obj.velocity.y = 0.0f;

	clamp_speed(p.move_speed, p.obj);

	if (is_key_down(SDLK_z) && p.cooldown <= 0.0f)
	{
		vec2 dir = vec2(0.0f, -1.0f);
		vec2 spawn_left = p.obj.position - vec2(0.0f, 16.0f);
		vec2 spawn_right = p.obj.position + vec2(p.obj.size.x, 0.0f) - vec2(0.0f, 16.0f);
		new_bullet(spawn_left, dir * p.bullet_speed, p.bullet_damage, vec2(0.0f));
		new_bullet(spawn_right, dir * p.bullet_speed, p.bullet_damage, vec2(0.0f));
		p.cooldown = p.fire_time;
	}

	update_world_object(p.obj, dt);
}

void render_enemy(Enemy &e)
{
	using namespace gfx2d;
	draw_rectangle(e.obj.position, e.obj.size, 0xff9944ff);

	Text text;
	text << int(e.hitpoints);
	draw_string(e.obj.position, text.getString());
}

void render_player(Player &p)
{
	using namespace gfx2d;
	draw_rectangle(p.obj.position, p.obj.size, 0x4499ffff);

	Text text;
	text << int(p.hitpoints);
	draw_string(p.obj.position, text.getString());
}

void render_bullet(Bullet &b)
{
	using namespace gfx2d;
	draw_fill_rectangle(b.obj.position, b.obj.size, 0xd3baffff);
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
	for (int i = 0; i < world.enemies.size(); i++)
	{
		if (world.enemies[i].dead)
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
	}
	gfx2d::end();
}