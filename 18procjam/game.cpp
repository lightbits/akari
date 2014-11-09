#include "game.h"

const int SCREEN_WIDTH = 180;
const int SCREEN_HEIGHT = 240;
int window_width;
int window_height;
Shader shader_resize;
RenderTexture rt_screen;
Mesh mesh_quad;
uint 
	tex_enemy,
	tex_player,
	tex_bullet;
Font font_debug;

const vec2 ENEMY_SIZE = vec2(32.0f, 32.0f);
const vec2 PLAYER_SPEED = vec2(128.0f, 128.0f);
const vec2 PLAYER_SIZE = vec2(32.0f, 32.0f);
const vec2 BULLET_SIZE = vec2(8.0f, 8.0f);
const vec2 BULLET_SPEED = vec2(0.0f, -64.0f);
const vec2 ENEMY_SPEED = vec2(64.0f, 64.0f);
const float ENEMY_MOVE_RANGE = 140.0f;

struct Entity
{
	vec2 position;
	vec2 velocity;
	vec2 size;
	uint texture;
};

vector<float>  enemy_cooldown;
vector<vec2>   enemy_move_to;
vector<Entity> enemies;
vector<Entity> bullets;
Entity player;

float random_from(float a, float b)
{
	return a + (b - a) * frand();
}

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;

	if (!load_texture_from_file(tex_enemy, "./enemy2.png", true, GL_NEAREST, GL_NEAREST) ||
		!load_texture_from_file(tex_player, "./player.png", true, GL_NEAREST, GL_NEAREST) ||
		!load_texture_from_file(tex_bullet, "./bullet.png", true, GL_NEAREST, GL_NEAREST))
		return false;

	if (!load_font(font_debug, "../data/fonts/proggytinyttsz_8x12.png"))
		return false;

	if (!shader_resize.load_from_file("./resize.vs", "./resize.fs"))
		return false;

	gfx2d::init(SCREEN_WIDTH, SCREEN_HEIGHT);
	rt_screen = gen_rendertexture(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB8);
	mesh_quad = Primitive::tex_quad();

	return true;
}

void free_game()
{
	
}

bool is_colliding(Entity &a, Entity &b)
{
	if (a.position.x > b.position.x + b.size.x ||
		a.position.x + a.size.x < b.position.x ||
		a.position.y > b.position.y + b.size.y ||
		a.position.y + a.size.y < b.position.y)
		return false;
	return true;
}

void spawn_enemy(vec2 position, vec2 velocity)
{
	Entity e;
	e.position = position;
	e.velocity = velocity;
	e.size = ENEMY_SIZE;
	e.texture = tex_enemy;
	enemies.push_back(e);
	enemy_cooldown.push_back(random_from(1.0f, 5.0f));
	enemy_move_to.push_back(position);
}

void spawn_bullet(vec2 position, vec2 velocity)
{
	Entity e;
	e.position = position;
	e.velocity = velocity;
	e.size = BULLET_SIZE;
	e.texture = tex_bullet;
	bullets.push_back(e);
}

void init_game()
{
	for (int i = 0; i < 8; i++)
	{
		float x = random_from(0.0f, SCREEN_WIDTH);
		float y = random_from(0.0f, SCREEN_HEIGHT - 64.0f);
		spawn_enemy(vec2(x, y), vec2(0.0f));
	}

	player.position = vec2(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 1.5f * PLAYER_SIZE.y);
	player.velocity = vec2(0.0f);
	player.texture = tex_player;
	player.size = PLAYER_SIZE;
}

void player_hit()
{
	//GAME_RUNNING = false;
}

void update_game(float dt)
{
	for (int i = 0; i < enemies.size(); i++)
	{
		if (enemy_cooldown[i] < 0.0f)
		{
			float x = random_from(ENEMY_SIZE.x, SCREEN_WIDTH - ENEMY_SIZE.x);
			float y = random_from(ENEMY_SIZE.y, ENEMY_MOVE_RANGE);
			enemy_move_to[i] = vec2(x, y);
			enemies[i].velocity = glm::normalize(enemy_move_to[i] - enemies[i].position) * ENEMY_SPEED;
			spawn_bullet(enemies[i].position, vec2(0.0f, 64.0f));
			enemy_cooldown[i] = 2.0f;
		}
		enemy_cooldown[i] -= dt;

		enemies[i].position += enemies[i].velocity * dt;
		if (glm::length(enemies[i].position - enemy_move_to[i]) < 1.0f)
			enemies[i].velocity = vec2(0.0f);

		if (is_colliding(player, enemies[i]))
			player_hit();
	}

	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i].position += bullets[i].velocity * dt;
		if (is_colliding(player, bullets[i]))
			player_hit();
		if (bullets[i].position.y > SCREEN_HEIGHT)
		{
			std::swap(bullets[i], bullets.back());
			bullets.pop_back();
			i--;
		}
	}

	if (is_key_down(SDLK_LEFT))
		player.velocity.x = -PLAYER_SPEED.x;
	else if (is_key_down(SDLK_RIGHT))
		player.velocity.x = +PLAYER_SPEED.x;
	else
		player.velocity.x = 0.0f;

	if (is_key_down(SDLK_UP))
		player.velocity.y = -PLAYER_SPEED.y;
	else if (is_key_down(SDLK_DOWN))
		player.velocity.y = +PLAYER_SPEED.y;
	else
		player.velocity.y = 0.0f;

	player.position += player.velocity * dt;
	if (player.position.x < 0.0f)
		player.position.x = 0.0f;
	else if (player.position.x + player.size.x > SCREEN_WIDTH)
		player.position.x = SCREEN_WIDTH - player.size.x;
	if (player.position.y < 0.0f)
		player.position.y = 0.0f;
	else if (player.position.y + player.size.y > SCREEN_HEIGHT)
		player.position.y = SCREEN_HEIGHT - player.size.y;
}

void render_game(float dt)
{
	glViewport(0, 0, rt_screen.width, rt_screen.height);
	glBindFramebuffer(GL_FRAMEBUFFER, rt_screen.fbo);
	gfx2d::begin();
	{
		using namespace gfx2d;
		blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
		clearc(0x44484Bff);

		for (int i = 0; i < enemies.size(); i++)
			tex_rectangle(enemies[i].position, enemies[i].size, enemies[i].texture);

		for (int i = 0; i < bullets.size(); i++)
			tex_rectangle(bullets[i].position, bullets[i].size, bullets[i].texture);

		tex_rectangle(player.position, player.size, tex_player);
	}
	gfx2d::end();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window_width, window_height);

	glBindTexture(GL_TEXTURE_2D, rt_screen.color);
	use_shader(shader_resize);
	uniform("tex", 0);
	mesh_quad.draw();
}