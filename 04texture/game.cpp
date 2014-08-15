#include "game.h"

Shader 
	shader_tex;
uint
	tex_test;
Mesh
	quad;
int window_width,
	window_height;

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;

	if (!load_texture_from_file(tex_test, "../data/fonts/proggytinyttsz_8x12.png"))
		return false;
	if (!shader_tex.load_from_file("../data/shaders/texture.vs", "../data/shaders/texture.fs"))
		return false;

	use_texture_2D(tex_test);
	tex_parameter(GL_NEAREST, GL_NEAREST);
	return true;
}

void free_game()
{

}

void init_game()
{
	quad = gen_tex_quad();
}

void update_game(float dt)
{

}

void render_game(float dt)
{
	blend_mode(true, GL_ONE, GL_ONE, GL_FUNC_ADD);
	clear(0x2a2a2aff, 1.0);

	use_shader(shader_tex);
	use_texture_2D(tex_test);
	uniform("tex", 0);
	quad.draw();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }