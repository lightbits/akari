#ifndef WORLD_H
#define WORLD_H
#include "kyoko.h"
#include "player.h"
#include "enemy.h"
#include "projectile.h"

void init_world(int width, int height);
void new_player(vec2 position, 
				vec2 velocity, 
				vec2 acceleration,
				float bullet_speed,
				float bullet_lifetime,
				int bullet_damage,
				float fire_time,
				float move_speed,
				int hitpoints);

void new_enemy(vec2 position, 
			   vec2 velocity, 
			   vec2 acceleration, 
			   function<void(Enemy&)> behaviour,
			   float move_speed, 
			   int hitpoints, 
			   float fire_time, 
			   float bullet_speed, 
			   int bullet_damage, 
			   float bullet_lifetime);

void new_bullet(uint shooter_id, 
				float lifetime, 
				vec2 position, 
				vec2 velocity, 
				int damage, 
				vec2 acceleration);

bool entity_fire_laser(uint id, Laser &laser, Entity &entity, Body &body);

bool collides(Body &a, Body &b);
bool is_offscreen(Body &b);
vector<Enemy> &get_enemies();
vector<Projectile> &get_bullets();
Player &get_player();
vec2 get_player_position();
Body get_player_body();

void update_world(float dt);
void render_world(float dt);

#endif