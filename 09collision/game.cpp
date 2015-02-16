#include "game.h"

struct Object
{
	vec2 position;
	vec2 size;
	vec2 velocity;
	float mass;
};

vector<Object> objects;

void add_object(float mass, float x, float y, float w, float h, float vx = 0.0f, float vy = 0.0f)
{
	Object obj;
	obj.mass = mass;
	obj.position = vec2(x, y);
	obj.size = vec2(w, h);
	obj.velocity = vec2(vx, vy);
	objects.push_back(obj);
}

int window_width;
int window_height;

Font font;

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;
	gfx2d::init(width, height);
	if (!load_font(font, "../data/fonts/proggytinyttsz_8x12.png"))
		return false;
	gfx2d::use_font(font);

	add_object(10.0f, 10.0f, 250.0f, 100.0f, 50.0f, 100.0f, 20.0f);
	add_object(800.0f, 400.0f, 250.0f, 50.0f, 80.0f, -100.0f, 30.0f);

	return true;
}

void free_game()
{
	gfx2d::dispose();
}

void init_game()
{
}

float intersection_line(float a0, float a1, float b0, float b1, float &begin, float &end)
{
	begin = max(a0, b0);
	end = min(a1, b1);
	return end - begin;
}

bool intersects(const Object &a, const Object &b, vec2 &displacement)
{
	float ax1 = a.position.x + a.size.x;
	float ax0 = a.position.x;
	float ay1 = a.position.y + a.size.y;
	float ay0 = a.position.y;

	float bx1 = b.position.x + b.size.x;
	float bx0 = b.position.x;
	float by1 = b.position.y + b.size.y;
	float by0 = b.position.y;

	bool out = ax1 <= bx0 || ax0 >= bx1 || ay1 <= by0 || ay0 >= by1;
	displacement = vec2(0.0f);

	float xbegin, xend;
	float idx = intersection_line(ax0, ax1, bx0, bx1, xbegin, xend);

	float ybegin, yend;
	float idy = intersection_line(ay0, ay1, by0, by1, ybegin, yend);

	float xsign = ax0 > bx0 ? 1.0f : -1.0f;
	float ysign = ay0 > by0 ? 1.0f : -1.0f;
	if (idx < idy)
		displacement.x = idx * xsign;
	else
		displacement.y = idy * ysign;
	return !out;
}

void resolve_1d(float m1, float m2, float v11, float v21, float *v12, float *v22)
{
	float dm = m1 - m2;
	float im = 1.0f / (m1 + m2);

	*v12 = (dm * v11 + 2.0f * m2 * v21) * im;
	*v22 = (-dm * v21 + 2.0f * m1 * v11) * im;
}

void resolve(Object &a, Object &b, vec2 displacement)
{
	a.position += displacement;
	b.position -= displacement;

	vec2 n = normalize(displacement);
	vec2 t = vec2(-n.y, n.x);

	float det = 1.0f / (t.x * n.y - t.y * n.x);
	vec2 va_proj = det * vec2(n.y * a.velocity.x - n.x * a.velocity.y, t.x * a.velocity.y - t.y * a.velocity.x);
	vec2 vb_proj = det * vec2(n.y * b.velocity.x - n.x * b.velocity.y, t.x * b.velocity.y - t.y * b.velocity.x);

	float n12, n22;
	resolve_1d(a.mass, b.mass, va_proj.y, vb_proj.y, &n12, &n22);

	a.velocity = va_proj.x * t + n12 * n;
	b.velocity = vb_proj.x * t + n22 * n;
}

vec2 selection_offset = vec2(0.0f, 0.0f);
bool dragging = false;
Object *selected = nullptr;
void update_game(float dt)
{
	if (is_key_down('r'))
		init_game();

	vec2i p = get_mouse_pos();
	for (int i = 0; i < objects.size(); i++)
	{
		if (!dragging &&
			is_mouse_down(MouseButtonLeft) &&
			p.x >= objects[i].position.x && 
			p.y >= objects[i].position.y &&
			p.x <= objects[i].position.x + objects[i].size.x &&
			p.y <= objects[i].position.y + objects[i].size.y)
		{
			selected = &objects[i];
			selection_offset = vec2(p) - objects[i].position;
			dragging = true;
		}

		if (dragging)
			continue;

		for (int j = 0; j < objects.size(); j++)
		{
			vec2 d;
			if (i != j && intersects(objects[i], objects[j], d))
				resolve(objects[i], objects[j], d);
		}
		objects[i].position += objects[i].velocity * dt;
	}

	if (!is_mouse_down(MouseButtonLeft))
	{
		dragging = false;
		selected = nullptr;
	}

	if (selected != nullptr && dragging)
	{
		selected->position = vec2(p) - selection_offset;
	}
}

void render_object(const Object &object)
{
	using namespace gfx2d;
	draw_rectangle(object.position, object.size, 0xffffffff);
	Text text;
	text << "x: " << object.velocity.x << ", y: " << object.velocity.y;
	text << '\n' << object.mass;
	draw_string(object.position + vec2(5.0f), text.getString());
}

void render_game(float dt)
{
	clearc(0x2a2a2aff);
	using namespace gfx2d;
	blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	begin();

	for (int i = 0; i < objects.size(); i++)
		render_object(objects[i]);
	end();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }