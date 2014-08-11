#include "game.h"

Shader 
	shader_font,
	shader_cube;
Font font;
Model cube;
mat4 
	mat_ortho,
	mat_perspective,
	mat_view;
int window_width,
	window_height;

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;

	if (!shader_font.load_from_file("../data/shaders/font.vs", "../data/shaders/font.fs") ||
		!shader_cube.load_from_file("./cube.vs", "./cube.fs"))
		return false;

	if (!load_font(font, "../data/fonts/proggytinyttsz_8x12.png"))
		return false;

	cube = gen_normal_cube();
	return true;
}

void free_game()
{
	shader_font.dispose();
	shader_cube.dispose();
	font.dispose();
}

void init_game()
{
	mat_ortho = ortho(0.0f, float(window_width), float(window_height), 0.0f, 0.0f, 1.0f);
	mat_perspective = perspective(PI / 4.0f, window_width / float(window_height), 0.1f, 5.0f);
}

void update_game(float dt)
{
	float t = get_elapsed_time();
	cube.mat = rotateY(t) * rotateX(t * 0.5f);
	mat_view = translate(0.0f, 0.0f, -2.0f);
}

void render_game(float dt)
{
	cull(true, GL_CCW, GL_BACK);
	clear(0x2a2a2aff, 1.0);

	use_shader(shader_cube);
	uniform("projection", mat_perspective);
	uniform("view", mat_view);
	uniform("time", get_elapsed_time());
	cube.draw();

	cull(false);
	use_shader(shader_font);
	use_font(font);
	uniform("projection", mat_ortho);
	blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	draw_string(window_width * 0.5f, window_height * 0.5f, "Hello world!", true);
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }