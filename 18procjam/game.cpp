#include "game.h"
#include <algorithm>
#include "world.h"

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

* Projectile behaviours
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

Font font;
int window_width;
int window_height;
uint tex;
float spawn_timer = 0.0f;
float spawn_period = 1.0f;

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

void behaviour_random_move(Enemy &e)
{
	float t = e.entity.lifetime;
	float period = 3.0f;
	int k = int(t / period);
	vec2 reference = 
		vec2(360.0f, 300.0f) * 
		(vec2(0.5f) + 
		0.5f * vec2(
		noise1f(e.id * 13 + 7 * k),
		noise1f(e.id * 127 + 511 * k)
		));
	pd_track(e.body, reference, 32.0f, 8.0f);
	entity_fire_laser(e.id, e.laser, e.entity, e.body);
}

void behaviour_aggressive(Enemy &e)
{
	float t = e.entity.lifetime;

	pd_track(e.body, path_1(t), 32.0f, 8.0f);

	if (t >= 2.3f)
	{
		float random_y_vel = noise1f(e.id) * 200.0f;
		e.body.velocity.y += random_y_vel;

		//float random_y_vel = noise1f(e.id) * 200.0f;
		//e.body.velocity.y = random_y_vel;
		//e.body.acceleration.y = 0.0f;

		vec2 direction = normalize(get_player_position() - e.body.position);
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
	init_world(width, height);

	return true;
}

void free_game()
{
	
}

void init_game()
{
	new_player(vec2(200.0f, 400.0f), vec2(0.0f), vec2(0.0f), PLAYER_BULLET_SPEED,
		PLAYER_BULLET_LIFETIME, PLAYER_BULLET_DAMAGE, PLAYER_FIRE_TIME, PLAYER_MOVE_SPEED,
		PLAYER_HITPOINTS);
}

void update_game(float dt)
{
	spawn_timer -= dt;
	if (spawn_timer <= 0.0f)
	{
		float spawn_x = -32.0f + (window_width + 64.0f) * frand();
		float spawn_y = -32.0f;
		new_enemy(vec2(spawn_x, spawn_y), vec2(0.0f), vec2(0.0f), behaviour_random_move,
			ENEMY_MOVE_SPEED, ENEMY_HITPOINTS, ENEMY_FIRE_TIME, 
			ENEMY_BULLET_SPEED, ENEMY_BULLET_DAMAGE, ENEMY_BULLET_LIFETIME);
		spawn_timer += spawn_period;
	}

	update_world(dt);
}

void render_game(float dt)
{
	gfx2d::begin();
	{
		using namespace gfx2d;
		clearc(BG_COLOR);
		blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		render_world(dt);

		Text text;
		text << "Enemies: " << get_enemies().size() << '\n';
		text << "Projectiles: " << get_bullets().size() << '\n';
		draw_string(5.0f, 5.0f, text.getString());
	}
	gfx2d::end();
}