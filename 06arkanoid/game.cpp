#include "game.h"
#define GLSL(src) "#version 150 core\n" #src

static const char *SHADER_3D_VS = GLSL(
	in vec3 position;
	in vec3 normal;
	out vec3 vNormal;
	uniform mat4 projection;
	uniform mat4 view;
	uniform mat4 model;

	void main()
	{
		vNormal = (model * vec4(normal, 0.0)).xyz;
		gl_Position = projection * view * model * vec4(position, 1.0);
	}
);

static const char *SHADER_3D_FS = GLSL(
	in vec3 vNormal;
	uniform vec4 color;
	out vec4 outColor;
	void main()
	{
		vec3 L = normalize(vec3(2.0, 8.0, 1.0));
		vec3 N = normalize(vNormal);
		//vec3 ambient = vec3(0.4, 0.55, 0.7);
		//vec3 ambient = vec3(0.2, 0.25, 0.3);
		vec3 ambient = 2.0 * vec3(0.3, 0.25, 0.2);
		float NdotL = max(dot(N, L), 0.0);
		outColor.rgb = NdotL * color.rgb + (1.0 - NdotL) * ambient * color.rgb;
		outColor.a = color.a;
	}
);

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

	bool destructible;
	int hitpoints;
	float anim_timer;
};

Object paddle;
Object ball;
vector<Object> objects;
int score;

int window_width;
int window_height;

Shader shader_3D;
Mesh mesh_cube;

const float PADDLE_WIDTH = 128.0f;
const float PADDLE_HEIGHT = 16.0f;
const float BALL_SIZE = 16.0f;
const float WALL_WIDTH = 8.0f;
const vec2 MAX_BALL_SPEED = vec2(550.0f, 650.0f);
const uint PADDLE_COLOR = 0xFDF0CEFF;
const uint BALL_COLOR = 0xD44A1BFF;
const uint BG_COLOR = 0x82A392FF;
const uint WALL_COLOR = 0xFDF0CEFF;
const uint NUM_BRICK_COLORS = 6;
const uint BRICK_COLOR[] = {
	0xC7D6C1FF,
	0xFBD6AAFF,
	0xE29374FF,
	0xCBB1B2FF,
	0xA6927AFF,
	0xD6D99FFF
};

Object gen_object(float x, float y, float width, float height, uint color, bool destructible = false, int hitpoints = 0)
{
	BoundingBox bb;
	Object obj;
	bb.position = vec2(x, y);
	bb.size = vec2(width, height);
	obj.bounds = bb;
	obj.velocity = vec2(0.0, 0.0);
	obj.color = color;
	obj.destructible = destructible;
	obj.hitpoints = hitpoints;
	obj.anim_timer = -1.0f;
	return obj;
}

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;
	gfx2d::init(width, height);

	if (!shader_3D.load_from_src(SHADER_3D_VS, SHADER_3D_FS))
		return false;

	mesh_cube = Primitive::normal_cube();

	return true;
}

void free_game()
{
	gfx2d::dispose();
}

void init_game()
{
	paddle = gen_object(window_width / 2.0f - PADDLE_WIDTH / 2.0f, window_height - 4.0f * PADDLE_HEIGHT, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_COLOR);
	ball = gen_object(window_width / 2.0f - BALL_SIZE / 2.0f, window_height - 8.0f * PADDLE_HEIGHT, BALL_SIZE, BALL_SIZE, BALL_COLOR);
	ball.velocity.y = 300.0f;
	ball.velocity.x = 300.0f;

	objects.clear();
	objects.push_back(gen_object(-64.0f, 0.0f, 64.0f, window_height, WALL_COLOR)); // Left
	objects.push_back(gen_object(window_width, 0.0f, 64.0f, window_height, WALL_COLOR)); // Right
	objects.push_back(gen_object(-64.0f, -64.0f, window_width + 128.0f, 64.0f, WALL_COLOR)); // Top
	objects.push_back(gen_object(-64.0f, window_height, window_width + 128.0f, 64.0f, WALL_COLOR)); // Bottom
	score = 0;

	float pad = 12.0f;
	int rows = 10;
	int columns = 11;
	float bw = (window_width - pad) / float(columns) - pad;
	float bh = 16.0f;
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			float pos_x = pad + x * (bw + pad);
			float pos_y = pad + y * (bh + pad);
			int i = (y * columns + x) % NUM_BRICK_COLORS;
			uint color = BRICK_COLOR[i];
			objects.push_back(gen_object(pos_x, pos_y, bw, bh, color, true, i + 1));
		}
	}
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

	bool out = ax1 <= bx0 || ax0 >= bx1 || ay1 <= by0 || ay0 >= by1;
	displacement = vec2(0.0f);

	//using namespace gfx2d;
	float xbegin, xend;
	float idx = intersection_line(ax0, ax1, bx0, bx1, xbegin, xend);
	//line(xbegin, ay1 + 10.0f, xend, ay1 + 10.0f, 0x7788ffff);

	float ybegin, yend;
	float idy = intersection_line(ay0, ay1, by0, by1, ybegin, yend);
	//line(ax1 + 10.0f, ybegin, ax1 + 10.0f, yend, 0x7788ffff);

	float xsign = ax0 > bx0 ? 1.0f : -1.0f;
	float ysign = ay0 > by0 ? 1.0f : -1.0f;
	if (idx < idy)
		displacement.x = idx * xsign;
	else
		displacement.y = idy * ysign;
	return !out;
}

void update_paddle(float dt)
{
	//float a = -700.0f;
	//float y0 = window_height - ball.bounds.position.y;
	//float v0 = -ball.velocity.y;
	//float t0 = (-v0 - sqrt(v0 * v0 - 2 * a * y0)) / a;
	//float t1 = (-v0 + sqrt(v0 * v0 - 2 * a * y0)) / a;
	//float t = max(t0, t1);
	//float x = ball.bounds.position.x + ball.velocity.x * t;
	//x = clamp(x, WALL_WIDTH, window_width - WALL_WIDTH);
	//float K = 1.5f;
	//float u = K * (x - paddle.bounds.position.x);
	//u = min(u, 500.0f);
	//paddle.velocity.x = u;

	if (is_key_down(SDLK_LEFT))
		paddle.velocity.x -= 1600.0f * dt;
	else if (is_key_down(SDLK_RIGHT))
		paddle.velocity.x += 1600.0f * dt;
	else
		paddle.velocity.x -= 10.0f * paddle.velocity.x * dt;

	if (is_key_down(SDLK_UP))
		paddle.velocity.y = 15.0f * (window_height - 64.0f - paddle.bounds.position.y);
	else
		paddle.velocity.y = 5.0f * (window_height - 16.0f - paddle.bounds.position.y);

	vec2 displacement;
	for (int i = 0; i < objects.size(); i++)
	{
		if (!intersects(paddle.bounds, objects[i].bounds, displacement))
			continue;
		paddle.bounds.position += displacement;

		vec2 reflect_axis = glm::normalize(displacement);
		vec2 new_direction = glm::normalize(glm::reflect(paddle.velocity, reflect_axis));
		paddle.velocity = glm::length(paddle.velocity) * new_direction;
	}

	paddle.bounds.position += paddle.velocity * dt;
}

void update_ball(float dt)
{
	ball.velocity.y += 200.0f * dt;
	vec2 displacement;
	if (intersects(ball.bounds, paddle.bounds, displacement))
	{
		ball.bounds.position += displacement;
		float sign = (ball.bounds.position.x  + ball.bounds.size.x * 0.5f) > (paddle.bounds.position.x + paddle.bounds.size.x * 0.5f) ? 1.0f : -1.0f;
		float mag = min(abs(paddle.velocity.x), 150.0f);
		vec2 reflect_axis = glm::normalize(displacement);
		vec2 new_direction = glm::normalize(glm::reflect(ball.velocity, reflect_axis) + vec2(sign * mag, 0.0f));
		ball.velocity = glm::length(ball.velocity) * new_direction;
		ball.velocity.y += 0.4f * paddle.velocity.y * dt;
	}

	for (int i = 0; i < objects.size(); i++)
	{
		Object &obj = objects[i];
		if (obj.destructible && obj.hitpoints <= 0)
			continue;

		if (!intersects(ball.bounds, obj.bounds, displacement))
			continue;

		ball.bounds.position += displacement;
		vec2 reflect_axis = glm::normalize(displacement);
		vec2 new_direction = glm::normalize(glm::reflect(ball.velocity, reflect_axis));
		ball.velocity = glm::length(ball.velocity) * new_direction;

		if (obj.destructible && obj.hitpoints > 0)
		{
			obj.hitpoints -= 1;
			if (obj.hitpoints <= 0)
			{
				obj.anim_timer = 1.0f;
				score++;
			}
		}
	}

	ball.velocity.x = min(ball.velocity.x, MAX_BALL_SPEED.x);
	ball.velocity.y = min(ball.velocity.y, MAX_BALL_SPEED.y);
	ball.bounds.position += ball.velocity * dt;
	ball.velocity.y -= 0.1f * ball.velocity.y * dt;
}

vec2 selection_offset = vec2(0.0f, 0.0f);
bool dragging = false;
BoundingBox *selected = nullptr;
void update_game(float dt)
{
	update_paddle(dt);
	update_ball(dt);

	if (is_key_down('r'))
		init_game();

	vec2i p = get_mouse_pos();
	for (int i = 0; i < objects.size(); i++)
	{
		if (!dragging &&
			is_mouse_down(MouseButtonLeft) &&
			p.x >= objects[i].bounds.position.x && 
			p.y >= objects[i].bounds.position.y &&
			p.x <= objects[i].bounds.position.x + objects[i].bounds.size.x &&
			p.y <= objects[i].bounds.position.y + objects[i].bounds.size.y)
		{
			selected = &objects[i].bounds;
			selection_offset = vec2(p) - objects[i].bounds.position;
			dragging = true;
		}
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
	draw_rectangle(object.bounds.position, object.bounds.size, object.color);
}

void render_symbol_3D(int n, vec2 position, uint color)
{
	const int nx = 3;
	const int ny = 5;
	string grid;
	if (n == 0)		 grid = "#### ## ## ####";
	else if (n == 1) grid = "##  #  #  # ###";
	else if (n == 2) grid = "###  #####  ###";
	else if (n == 3) grid = "###  ####  ####";
	else if (n == 4) grid = "# ## ####  #  #";
	else if (n == 5) grid = "####  ###  ####";
	else if (n == 6) grid = "####  #### ####";
	else if (n == 7) grid = "###  #  # #  # ";
	else if (n == 8) grid = "#### ##### ####";
	else if (n == 9) grid = "#### ####  #  #";
	
	const vec3 size = vec3(0.05f);
	for (int y = 0; y < ny; y++)
	{
		for (int x = 0; x < nx; x++)
		{
			if (grid[y * nx + x] == ' ')
				continue;
			float px = -1.0f + 2.0f * position.x / window_width + x * size.x;
			float py = 0.2f + (ny - y - 1) * size.y;
			float pz = -1.0f + 2.0f * position.y / window_height;
			uniform("model", translate(px, py, pz) * scale(size));
			uniform("color", to_rgba(color));
			glDrawElements(GL_TRIANGLES, mesh_cube.num_indices, GL_UNSIGNED_INT, 0);
		}
	}
}

void render_number_3D(int n, vec2 position, uint color)
{
	int i = 0;
	do
	{
		render_symbol_3D(n % 10, position - vec2(i * 60.0f, 0.0f), color);
		i++;
		n = n / 10;
	} while(n > 0);
}

void render_object_3D(const Object &object)
{
	vec2 res = vec2(window_width, window_height);
	vec2 pos = 2.0f * object.bounds.position / res - vec2(1.0f);
	vec2 size = object.bounds.size / res;
	float w = 2.0f * size.x;
	float h = 0.05f;
	float d = 2.0f * size.y;
	float x = pos.x + w * 0.5f;
	float y = 0.0f;
	float z = pos.y + d * 0.5f;
	mat4 mat_model = translate(x, y, z) * scale(w, h, d);
	uniform("model", mat_model);
	uniform("color", to_rgba(object.color));
	glDrawElements(GL_TRIANGLES, mesh_cube.num_indices, GL_UNSIGNED_INT, 0);
}

void render_visualizers(float dt)
{
	float norm_v = glm::length(ball.velocity) / glm::length(MAX_BALL_SPEED);
	Object speed_bar = gen_object(0.0f, -128.0f, window_width * norm_v, 16.0f, WALL_COLOR);
	render_object_3D(speed_bar);
}

void render_3D(float dt)
{
	float paddle_x = 2.0f * paddle.bounds.position.x / window_width - 1.0f;
	float ball_y = 2.0f * ball.bounds.position.y / window_height - 1.0f;
	float ball_x = 2.0f * ball.bounds.position.x / window_width - 1.0f;
	float roty = 0.13f * paddle_x;
	float rotx = 0.07f * ball_y;
	float zoom = -0.1f * cos(ball_x * PI / 2.0f);
	
	mat4 mat_projection = perspective(PI / 3.0f + zoom, window_width / float(window_height), 0.5f, 5.0f);
	mat4 mat_view = translate(0.0f, 0.0f, -2.5f) * rotateX(-0.8f + rotx) * rotateY(roty);
	use_shader(shader_3D);
	depth_write(true);
	depth_test(true, GL_LEQUAL);
	clear(BG_COLOR, 1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, mesh_cube.vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_cube.index_buffer);
	attribfv("position", 3, 6, 0);
	attribfv("normal", 3, 6, 3);
	uniform("projection", mat_projection);
	uniform("view", mat_view);

	for (int i = 0; i < objects.size(); i++)
	{
		Object &obj = objects[i];
		if (obj.destructible && obj.hitpoints <= 0 && obj.anim_timer > 0.0f)
		{
			// play explosion anim
			int i = int(obj.anim_timer * 10.0f) % NUM_BRICK_COLORS;
			obj.color = BRICK_COLOR[i];
			obj.anim_timer -= dt;
		}
		else if (obj.destructible && obj.hitpoints <= 0 && obj.anim_timer <= 0.0f)
			continue;
		render_object_3D(obj);
	}
	render_object_3D(paddle);
	render_object_3D(ball);
	render_visualizers(dt);

	render_number_3D(score, vec2(200.0f, -128.0f), WALL_COLOR);
}

void render_game(float dt)
{
	render_3D(dt);
	//clearc(0x2a2a2aff);
	//using namespace gfx2d;
	//begin();
	//render_object(paddle);
	//render_object(ball);

	//for (int i = 0; i < objects.size(); i++)
	//{
	//	render_object(objects[i]);
	//	for (int j = 0; j < objects.size(); j++)
	//	{
	//		if (i == j)
	//			continue;

	//		vec2 displacement;
	//		if (intersects(objects[i].bounds, objects[j].bounds, displacement))
	//		{
	//			vec2 center = objects[i].bounds.position + objects[i].bounds.size * 0.5f;
	//			line(center, center + displacement, 0xff8877ff);
	//			draw_rectangle(objects[i].bounds.position, objects[i].bounds.size, 0xff7755ff);
	//		}
	//		else
	//		{
	//			draw_rectangle(objects[i].bounds.position, objects[i].bounds.size, 0xffffffff);
	//		}
	//	}
	//}
	//end();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }