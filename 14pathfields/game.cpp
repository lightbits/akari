/*
Using potential fields for pathfinding is a nice alternative to traditional source-all
or all-to-all optimal path search algorithms (A*, Dijkstra, Floyd-Warshall, ...), when
you have a ton of paths to calculate.

http://www.cs.mcgill.ca/~hsafad/robotics/
http://en.wikipedia.org/wiki/Motion_planning#Potential_Fields
http://aiguy.org/onewebmedia/CIG12_PFsInStarcraft.pdf
http://aigamedev.com/open/tutorials/potential-fields/
http://gamedev.stackexchange.com/questions/69689/pathfinding-in-non-tile-based-2d-map-maybe-potential-field
http://gamedevelopment.tutsplus.com/tutorials/understanding-goal-based-vector-field-pathfinding--gamedev-9007
*/

#include "game.h"

Font font;
const int res_x = 4;
const int res_y = 4;
int window_width, window_height;

bool load_game(int width, int height)
{
	if (!load_font(font, "../data/fonts/proggytinyttsz_8x12.png"))
		return false;
	window_width = width;
	window_height = height;
	return true;
}

void free_game()
{
	font.dispose();
}

void init_game()
{
	gfx2d::init(window_width, window_height);
	gfx2d::use_font(font);
}

void update_game(float dt)
{
	
}

void render_game(float dt)
{
	clearc(0x2a2a2aff);
	blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
	{ using namespace gfx2d;
		begin();
		draw_string(5.0f, 5.0f, "Hello World!");
		end();
	}
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }