#include "game.h"

struct BoundingBox
{
	vec2 position;
	vec2 size;
};

struct Object
{
	BoundingBox bounds;
	vec2 velocity;
	uint color;
};

Object paddle;
Object ball;
Object wall_top, wall_left, wall_right;
vector<Object> bricks;

int window_width;
int window_height;

const float PADDLE_WIDTH = 128.0f;
const float PADDLE_HEIGHT = 16.0f;
const float BALL_WIDTH = 16.0f;
const float BALL_HEIGHT = 16.0f;
const uint PADDLE_COLOR = 0xffffffff;
const uint BALL_COLOR = 0xff7785ff;

Object gen_object(float x, float y, float width, float height, uint color)
{
	BoundingBox bb;
	Object obj;
	bb.position = vec2(x, y);
	bb.size = vec2(width, height);
	obj.bounds = bb;
	obj.velocity = vec2(0.0, 0.0);
	obj.color = color;
	return obj;
}

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;
	gfx2d::init(width, height);
	return true;
}

void free_game()
{
	gfx2d::dispose();
}

void init_game()
{
	paddle = gen_object(window_width / 2.0f - PADDLE_WIDTH / 2.0f, window_height - 2.0f * PADDLE_HEIGHT, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_COLOR);
	ball = gen_object(window_width / 2.0f - BALL_WIDTH / 2.0f, window_height / 2.0f - BALL_HEIGHT / 2.0f, BALL_WIDTH, BALL_HEIGHT, BALL_COLOR);
	ball.velocity.y = 300.0f;

	bricks.clear();
	float pad = 8.0f;
	float w = 44.0f;
	float h = 24.0f;
	int Nx = (640 - pad) / (w + pad);
	for (int i = 0; i < 60; i++)
	{		
		float x = pad + (i % Nx) * (w + pad);
		float y = pad + (i / Nx) * (h + pad);
		uint8 r = uint8((i / 59.0f) * 255.0f);
		uint8 g = uint8((i / 100.0f) * 255.0f);
		uint8 b = uint8((i / 128.0f) * 255.0f);
		uint color = 0xff | (b << 8) | (g << 16) | (r << 24);
		Object brick = gen_object(x, y, w, h, color);
		bricks.push_back(brick);
	}
	//bricks.push_back(gen_object(10.0f, 10.0f, 100.0f, 60.0f, 0x4455ffff));
	//bricks.push_back(gen_object(10.0f, 80.0f, 70.0f, 90.0f, 0xff7755ff));
}

float intersection_line(float a0, float a1, float b0, float b1, float &begin, float &end)
{
	begin = max(a0, b0);
	end = min(a1, b1);
	return end - begin;
}

bool intersects(const BoundingBox &a, const BoundingBox &b, vec2 &displacement)
{
	float ax1 = a.position.x + a.size.x;
	float ax0 = a.position.x;
	float ay1 = a.position.y + a.size.y;
	float ay0 = a.position.y;

	float bx1 = b.position.x + b.size.x;
	float bx0 = b.position.x;
	float by1 = b.position.y + b.size.y;
	float by0 = b.position.y;

	bool out = ax1 < bx0 || ax0 > bx1 || ay1 < by0 || ay0 > by1;
	displacement = vec2(0.0f);

	float xbegin, xend;
	float idx = intersection_line(ax0, ax1, bx0, bx1, xbegin, xend);
	//line(xbegin, ay1 + 10.0f, xend, ay1 + 10.0f, 0x7788ffff);

	float ybegin, yend;
	float idy = intersection_line(ay0, ay1, by0, by1, ybegin, yend);
	//line(ax1 + 10.0f, ybegin, ax1 + 10.0f, yend, 0x7788ffff);

	if (idx < idy)
		displacement.x = idx;
	else
		displacement.y = idy;
	return !out;
}

void update_paddle(float dt)
{
	if (is_key_down(SDLK_LEFT))
		paddle.velocity.x -= 1600.0f * dt;
	else if (is_key_down(SDLK_RIGHT))
		paddle.velocity.x += 1600.0f * dt;
	else
		paddle.velocity.x -= 10.0f * paddle.velocity.x * dt;

	paddle.bounds.position += paddle.velocity * dt;
}

void update_ball(float dt)
{
	//ball.velocity.y += 700.0f * dt;
	vec2 displacement;
	if (intersects(ball.bounds, paddle.bounds, displacement))
	{
		ball.bounds.position -= displacement;
		float sign = (ball.bounds.position.x  + ball.bounds.size.x * 0.5f) > (paddle.bounds.position.x + paddle.bounds.size.x * 0.5f) ? 1.0f : -1.0f;
		float mag = min(abs(paddle.velocity.x), 150.0f);
		ball.velocity.x += sign * mag * 0.05f;
		ball.velocity.y *= -1.0f;
	}

	if (ball.bounds.position.x < 0.0f)
	{
		ball.bounds.position.x = 0.0f;
		ball.velocity.x *= -1.0f;
	}
	if (ball.bounds.position.x + ball.bounds.size.x > window_width)
	{
		ball.bounds.position.x = window_width - ball.bounds.size.x;
		ball.velocity.x *= -1.0f;
	}

	ball.bounds.position += ball.velocity * dt;
}

//vec2 selection_offset = vec2(0.0f, 0.0f);
//bool dragging = false;
//BoundingBox *selected = nullptr;
void update_game(float dt)
{
	update_paddle(dt);
	update_ball(dt);

	if (is_key_down('r'))
		init_game();

	//vec2i p = get_mouse_pos();
	//for (int i = 0; i < bricks.size(); i++)
	//{
	//	if (!dragging &&
	//		is_mouse_down(MouseButtonLeft) &&
	//		p.x >= bricks[i].bounds.position.x && 
	//		p.y >= bricks[i].bounds.position.y &&
	//		p.x <= bricks[i].bounds.position.x + bricks[i].bounds.size.x &&
	//		p.y <= bricks[i].bounds.position.y + bricks[i].bounds.size.y)
	//	{
	//		selected = &bricks[i].bounds;
	//		selection_offset = vec2(p) - bricks[i].bounds.position;
	//		dragging = true;
	//	}
	//}

	//if (!is_mouse_down(MouseButtonLeft))
	//{
	//	dragging = false;
	//	selected = nullptr;
	//}

	//if (selected != nullptr && dragging)
	//{
	//	selected->position = vec2(p) - selection_offset;
	//}
}

void render_object(const Object &object)
{
	using namespace gfx2d;
	fill_rectangle(object.bounds.position, object.bounds.size, object.color);
}

void render_game(float dt)
{
	clearc(0x2a2a2aff);
	using namespace gfx2d;
	begin();
	render_object(paddle);
	render_object(ball);

	for (int i = 0; i < bricks.size(); i++)
	{
		render_object(bricks[i]);
		//for (int j = 0; j < bricks.size(); j++)
		//{
		//	if (i == j)
		//		continue;

		//	vec2 displacement;
		//	if (intersects(bricks[i].bounds, bricks[j].bounds, displacement))
		//	{
		//		vec2 center = bricks[i].bounds.position + bricks[i].bounds.size * 0.5f;
		//		line(center, center + displacement, 0xff8877ff);
		//		rectangle(bricks[i].bounds.position, bricks[i].bounds.size, 0xff7755ff);
		//	}
		//	else
		//	{
		//		rectangle(bricks[i].bounds.position, bricks[i].bounds.size, 0xffffffff);
		//	}
		//}
	}
	end();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }