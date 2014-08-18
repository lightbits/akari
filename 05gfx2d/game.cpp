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

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;

	if (!load_texture_from_file(tex_test, "../data/textures/tex0.png", GL_NEAREST, GL_NEAREST))
		return false;

	if (!load_font(font_debug, "../data/fonts/proggytinyttsz_8x12.png"))
		return false;

	gfx2d::init(width, height);
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
	
}

void draw_textbox(float x, float y, float width, float height)
{
	width -= 1.0f;
	height -= 1.0f;
	using namespace gfx2d;
	uint color_bg = 0x00003EFF;
	uint color_hi = 0x69656DFF;
	uint color_lo = 0x343255FF;
	fill_rectangle(x, y, width, height, color_bg);
	line(x, y, x + width, y, color_lo);
	line(x, y, x, y + height, color_lo);
	line(x, y + height, x + width, y + height, color_hi);
	line(x + width, y, x + width, y + height, color_hi);
}

void draw_panel(float x, float y, float width, float height)
{
	width -= 1.0f;
	height -= 1.0f;
	using namespace gfx2d;
	uint color_sp = 0x837E78FF;
	uint color_hi = 0x69656DFF;
	uint color_lo = 0x343255FF;
	line(x, y, x + width, y, color_hi);
	line(x, y, x, y + height, color_hi);
	line(x, y + height, x + width, y + height, color_lo);
	line(x + width, y, x + width, y + height, color_lo);
	fill_rectangle(x + 1.0f, y + 1.0f, 1.0f, 1.0f, color_sp);
	fill_rectangle(x + width - 3.0f, y + 1.0f, 1.0f, 1.0f, color_sp);
	fill_rectangle(x + 1.0f, y + height - 3.0f, 1.0f, 1.0f, color_sp);
	fill_rectangle(x + width - 3.0f, y + height - 3.0f, 1.0f, 1.0f, color_sp);
}

void render_game(float dt)
{
	using namespace gfx2d;
	blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
	glLineWidth(1.0f);
	clearc(0x4E4B61FF);
	begin();
	draw_textbox(10.0f, 60.0f, 64.0f, 14.0f);
	draw_panel(100.0f, 160.0f, 200.0f, 100.0f);
	draw_panel(300.0f, 160.0f, 100.0f, 100.0f);
	tex_rectangle(101.0f, 161.0f, 198.0f, 98.0f, tex_test);

	use_font(font_debug);
	draw_string(5.0f, 5.0f, "Hello World!");
	end();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }