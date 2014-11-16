#include "game.h"
#include <algorithm>

const uint BG_COLOR = 0x404968ff;

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

void new_enemy(float fire_time, float move_speed, float bullet_speed, vec2 position, vec2 velocity, vec2 acceleration)
{
	Enemy e;
	e.fire_time = fire_time;
	e.move_speed = move_speed;
	e.bullet_speed = bullet_speed;
	e.cooldown = fire_time;
	e.hitpoints = 10;
	e.bullet_damage = 10;
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

void init_player(float fire_time, float move_speed, float bullet_speed, vec2 position, vec2 velocity, vec2 acceleration)
{
	world.player.fire_time = fire_time;
	world.player.move_speed = move_speed;
	world.player.bullet_speed = bullet_speed;
	world.player.cooldown = 0.0f;
	world.player.hitpoints = 10;
	world.player.bullet_damage = 5;
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

	// Clamp speed
	float speed = length(p.obj.velocity);
	if (speed >= p.move_speed)
		p.obj.velocity = normalize(p.obj.velocity) * p.move_speed;

	if (is_key_down(SDLK_z) && p.cooldown <= 0.0f)
	{
		vec2 dir = vec2(0.0f, -1.0f);
		vec2 spawn_left = p.obj.position - vec2(0.0f, 8.0f);
		vec2 spawn_right = p.obj.position + vec2(p.obj.size.x, 0.0f) - vec2(0.0f, 8.0f);
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
}

void render_bullet(Bullet &b)
{
	using namespace gfx2d;
	draw_fill_rectangle(b.obj.position, b.obj.size, 0x55bb77ff);
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
	new_enemy(2.0f, 300.0f, 400.0f, vec2(200.0f, 200.0f), vec2(0.0f, 0.0f), vec2(0.0f, 0.0f));
	new_enemy(2.0f, 100.0f, 200.0f, vec2(90.0f, 30.0f), vec2(50.0f, 10.0f), vec2(0.0f, -10.0f));
	new_enemy(2.0f, 100.0f, 200.0f, vec2(140.0f, 30.0f), vec2(50.0f, 10.0f), vec2(0.0f));
	new_enemy(2.0f, 100.0f, 200.0f, vec2(180.0f, 30.0f), vec2(50.0f, 10.0f), vec2(0.0f));
	new_enemy(2.0f, 100.0f, 200.0f, vec2(220.0f, 30.0f), vec2(50.0f, 10.0f), vec2(0.0f));
	init_player(0.2f, 200.0f, 300.0f, vec2(200.0f, 400.0f), vec2(0.0f), vec2(0.0f));
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

		draw_rectangle(vec2(get_mouse_pos()) - vec2(8.0f), vec2(16.0f), 0xffffffff);

		draw_string(5.0f, 5.0f, "Hello World!");
	}
	gfx2d::end();
}