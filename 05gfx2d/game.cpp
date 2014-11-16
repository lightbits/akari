#include "game.h"

unordered_map<string, Shader> shaders;
bool load_shader(string name, string vs, string fs)
{
	Shader shader;
	if (!shader.load_from_file(vs, fs))
		return false;
	shaders[name] = shader;
	return true;
}
void use_shader(string name)
{
	if (shaders.find(name) == shaders.end())
		return;
	use_shader(shaders[name]);
}

int
	window_width,
	window_height;
mat4
	mat_ortho;
uint
	tex_test;
Font
	font_debug;

struct Star
{
	uint color;
	vec2 position;
	vec2 velocity;
};

vector<Star> stars;
int max_num_stars = 128;
int num_star_colors = 3;
uint star_colors[] = {
	0xE5E7CAFF,
	0xB4B4AAFF,
	0x6F6F8FFF
};

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;

	if (!load_texture_from_file(tex_test, "../data/textures/tex0.png", GL_NEAREST, GL_NEAREST))
		return false;

	if (!load_font(font_debug, "../data/fonts/proggytinyttsz_8x12.png"))
		return false;

	gfx2d::init(width, height);

	for (int i = 0; i < max_num_stars; i++)
	{
		Star star;
		int layer = i % num_star_colors;
		star.color = star_colors[layer];
		star.position = vec2(frand(), frand()) * vec2(window_width, window_height);
		star.velocity = vec2(-250.0f + layer * 48.0f + 24.0f * frand(), 0.0f);
		stars.push_back(star);
	}

	return true;
}

void free_game()
{
	
}

void init_game()
{
	mat_ortho = ortho(0.0f, float(window_width), float(window_height), 0.0f, 0.0f, 1.0f);
}

void update_game(float dt)
{
	for (int i = 0; i < stars.size(); i++)
	{
		Star &star = stars[i];
		if (star.position.x < 0.0f)
			star.position = vec2(window_width + 5.0f, frand() * window_height);
		else
			star.position += star.velocity * dt;
	}
}

void draw_textbox(float x, float y, float width, float height)
{
	width -= 1.0f;
	height -= 1.0f;
	using namespace gfx2d;
	uint color_bg = 0x00003EFF;
	uint color_hi = 0x69656DFF;
	uint color_lo = 0x343255FF;
	draw_fill_rectangle(x, y, width, height, color_bg);
	draw_line(x, y, x + width, y, color_lo);
	draw_line(x, y, x, y + height, color_lo);
	draw_line(x, y + height, x + width, y + height, color_hi);
	draw_line(x + width, y, x + width, y + height, color_hi);
}

void draw_panel(float x, float y, float width, float height)
{
	width -= 1.0f;
	height -= 1.0f;
	using namespace gfx2d;
	uint color_sp = 0x837E78FF;
	uint color_hi = 0x69656DFF;
	uint color_lo = 0x343255FF;
	draw_line(x, y, x + width, y, color_hi);
	draw_line(x, y, x, y + height, color_hi);
	draw_line(x, y + height, x + width, y + height, color_lo);
	draw_line(x + width, y, x + width, y + height, color_lo);
	draw_fill_rectangle(x + 1.0f, y + 1.0f, 1.0f, 1.0f, color_sp);
	draw_fill_rectangle(x + width - 3.0f, y + 1.0f, 1.0f, 1.0f, color_sp);
	draw_fill_rectangle(x + 1.0f, y + height - 3.0f, 1.0f, 1.0f, color_sp);
	draw_fill_rectangle(x + width - 3.0f, y + height - 3.0f, 1.0f, 1.0f, color_sp);
}

void render_game(float dt)
{
	using namespace gfx2d;
	blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
	glLineWidth(1.0f);
	clearc(0x001333ff);
	//clearc(0x4E4B61FF);
	begin();

	for (int i = 0; i < stars.size(); i++)
		draw_fill_rectangle(stars[i].position - vec2(2.0f), vec2(4.0f), stars[i].color);

	//draw_textbox(10.0f, 60.0f, 64.0f, 14.0f);
	//draw_panel(100.0f, 160.0f, 200.0f, 100.0f);
	//draw_panel(300.0f, 160.0f, 100.0f, 100.0f);
	//draw_tex_rectangle(101.0f, 161.0f, 198.0f, 98.0f, tex_test);

	//use_font(font_debug);
	//draw_string(5.0f, 5.0f, "Hello World!");
	end();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }