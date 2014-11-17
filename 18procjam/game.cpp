#include "game.h"
#include <algorithm>
using std::function;

const uint	BG_COLOR				= 0x404968ff;
const int	PLAYER_BULLET_DAMAGE	= 5;
const int	PLAYER_HITPOINTS		= 10;
const float PLAYER_MOVE_SPEED		= 200.0f;
const float PLAYER_BULLET_SPEED		= 300.0f;
const float PLAYER_BULLET_LIFETIME	= 5.0f;
const float PLAYER_FIRE_TIME		= 0.2f;
const int	ENEMY_BULLET_DAMAGE		= 10;
const int	ENEMY_HITPOINTS			= 10;
const float ENEMY_BULLET_SPEED		= 300.0f;
const float ENEMY_BULLET_LIFETIME	= 2.0f;
const float ENEMY_MOVE_SPEED		= 200.0f;
const float ENEMY_FIRE_TIME			= 2.0f;


/*
TODO
* Rename Entity to something else. Make Entity a base class for all game objects
* Archetypal types?
* Make general weapon thing
* Add laser body? Or should this be given in craft-body
* Rotations, scaling
* Arbitrary 2D polygons
* Combine Player and Enemy types
* Add paths
	Arc(radius, t)
	Sine(...)
	Linear(...)
	Piecewise add paths

* Bullet behaviours
	Missiles (drop, wait, launch)

* Improve rendering
	Avoid resubmitting all vertices each frame!!
	Keep vertex data to be rendered in a buffer
	Do some magic to remove/change stuff
	?
	Or some culling mechanism, octrees

* Physics
	ApplyImpulse(force, ..., body)
	world.add_force (duration)
	for f in forces:
		update f

struct Body
{
	position, velocity, ...
	shape (some polygon shape!)
};

Behaviours control when/where to go, when/where to fire weapons
animation, etc...
Should also handle spawning and death (anims)

Combined behaviours
struct Behaviour
{
	Coroutine init;
	vector<Coroutine> coroutines; // Coroutines that execute in disjunct time intervals
	Coroutine terminate;
};
*/

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
	function<void(Enemy&)> behaviour;
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
float spawn_period = 0.4f;

void new_enemy(vec2 position, vec2 velocity, vec2 acceleration, function<void(Enemy&)> behaviour)
{
	Entity entity;
	entity.move_speed = ENEMY_MOVE_SPEED;
	entity.hitpoints = ENEMY_HITPOINTS;
	entity.lifetime = 0.0f;
	entity.dead = false;

	Body body;
	body.size = vec2(32.0f);

	// These should be handled by the behaviour routine
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

	Bullet bullet;
	bullet.body = body;
	bullet.dead = false;
	bullet.damage = damage;
	bullet.lifetime = lifetime;
	bullet.shooter_id = shooter_id;
	world.bullets.push_back(bullet);
}

bool is_offscreen(Body &b)
{
	return (b.position.x + b.size.x < 0.0f ||
		b.position.x > window_width ||
		b.position.y + b.size.y < 0.0f ||
		b.position.y > window_height);
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

	if (is_offscreen(b.body))
		b.dead = true;
}

void clamp_speed(float max_speed, Body &body)
{
	float speed = length(body.velocity);
	if (speed > max_speed)
		body.velocity = normalize(body.velocity) * max_speed;
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
	clamp_speed(e.entity.move_speed, e.body);
	e.entity.lifetime += dt;
	if (is_offscreen(e.body))
		e.entity.dead = true;

	e.behaviour(e);
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

vec2 path_1(float t)
{
	if (t < 2.3f)
	{
		return vec2(300.0f, 300.0f);
	}
	else
	{
		t -= 2.3f;
		return vec2(300.0f - 300.0f * t / 4.0f, 300.0f);
	}
}

vec2 path_2(float t)
{
	return vec2(200.0f, window_height * (t / 10.0f));
}

void pd_track(Body &b, vec2 r, float kp, float kd)
{
	b.acceleration = -kp * (b.position - r) - kd * b.velocity;
}

void behaviour_aggressive(Enemy &e)
{
	float t = e.entity.lifetime;

	pd_track(e.body, path_1(t), 32.0f, 8.0f);

	if (t >= 2.3f)
	{
		//float random_y_vel = noise1f(e.id) * 200.0f;
		//e.body.velocity.y += random_y_vel;

		vec2 direction = normalize(world.player.body.position - e.body.position);
		if (entity_fire_laser(e.id, e.laser, e.entity, e.body))
		{
			// something?
			// recoil maybe
		}
	}

	
}

void behaviour_enemy_default(Enemy &e)
{

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
	init_player(vec2(200.0f, 400.0f), vec2(0.0f), vec2(0.0f));
}

void update_game(float dt)
{
	spawn_timer -= dt;
	if (spawn_timer <= 0.0f)
	{
		float spawn_x = -32.0f + (window_width + 64.0f) * frand();
		float spawn_y = -32.0f;
		new_enemy(vec2(spawn_x, spawn_y), vec2(0.0f), vec2(0.0f), behaviour_aggressive);
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