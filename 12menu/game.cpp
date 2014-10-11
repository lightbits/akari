#include "game.h"

Font font;

bool load_game(int width, int height)
{
	if (!load_font(font, "../data/fonts/proggytinyttsz_8x12.png"))
		return false;
	gfx2d::init(width, height);
	gfx2d::use_font(font);
	return true;
}

void free_game()
{
	font.dispose();
}

void init_game()
{
	
}

void update_game(float dt)
{
	
}

void render_game(float dt)
{
	using namespace gfx2d;
	begin();
	draw_string(5.0f, 5.0f, "Hello World!");
	end();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }