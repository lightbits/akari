#include "game.h"

//class Tilemap
//{
//public:
//	Tilemap() { _grid = nullptr; _width = 0; _height = 0; }
//	void init(int width, int height, int *grid = nullptr)
//	{
//		dispose();
//		_width = width;
//		_height = height;
//		_grid = new int[_height * _width];
//		if (grid != nullptr)
//		{
//			for (int i = 0; i < _width * _height; i++)
//				_grid[i] = grid[i];
//		}
//		else
//		{
//			for (int i = 0; i < _height * _width; i++)
//				_grid[i] = 0;
//		}
//	}
//	void dispose() 
//	{
//		if (_grid != nullptr)
//			delete[] _grid;
//		_grid = nullptr;
//		_width = 0;
//		_height = 0;
//	}
//	int set(int x, int y, int t)
//	{
//		ASSERT(x >= 0 && x < _width && y >= 0 && y < _height, "Tilemap indices out of range");
//		_grid[y * _height + x] = t;
//	}
//	int get(int x, int y)
//	{
//		ASSERT(x >= 0 && x < _width && y >= 0 && y < _height, "Tilemap indices out of range");
//		return _grid[y * _height + x];
//	}
//	int width() { return _width; }
//	int height() { return _height; }
//private:
//	int *_grid;
//	int _width;
//	int _height;
//};
//
//Tilemap tilemap;
uint texture;
uint tilemap;
Mesh mesh_grid;
Shader shader_tilemap;
const int res_x = 4;
const int res_y = 4;

bool load_game(int width, int height)
{
	if (!load_texture_from_file(texture, "./tiles_cavestory.bmp", false, GL_NEAREST, GL_NEAREST))
		return false;
	if (!shader_tilemap.load_from_file("./tilemap.vs", "./tilemap.fs"))
		return false;

	// Generate grid mesh
	struct Vertex
	{
		vec2 position;
		vec2 texel;
		Vertex(vec2 p, vec2 t) : position(p), texel(t) { }
		Vertex() { }
	};
	Vertex *vertex_data = new Vertex[res_x * res_y * 4];
	uint *index_data = new uint[res_x * res_y * 6];
	int index_v = 0;
	int index_i = 0;
	float dx = 1.0f / res_x;
	float dy = 1.0f / res_y;
	for (int yi = 0; yi < res_y; yi++)
	{
		for (int xi = 0; xi < res_x; xi++)
		{
			float x = xi * dx;
			float y = yi * dy;
			vec2 v00 = vec2(x, y);
			vec2 v10 = vec2(x + dx, y);
			vec2 v01 = vec2(x, y + dy);
			vec2 v11 = vec2(x + dx, y + dy);
			index_data[index_i++] = index_v;
			index_data[index_i++] = index_v + 1;
			index_data[index_i++] = index_v + 2;
			index_data[index_i++] = index_v + 2;
			index_data[index_i++] = index_v + 3;
			index_data[index_i++] = index_v;
			vertex_data[index_v++] = Vertex(2.0f * v00 - vec2(1.0f), v00);
			vertex_data[index_v++] = Vertex(2.0f * v10 - vec2(1.0f), v00);
			vertex_data[index_v++] = Vertex(2.0f * v11 - vec2(1.0f), v00);
			vertex_data[index_v++] = Vertex(2.0f * v01 - vec2(1.0f), v00);
		}
	}
	mesh_grid.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, res_x * res_y * 4 * sizeof(Vertex), vertex_data, GL_STATIC_DRAW);
	mesh_grid.index_buffer = gen_buffer(GL_ELEMENT_ARRAY_BUFFER, res_x * res_y * 6 * sizeof(uint), index_data, GL_STATIC_DRAW);
	mesh_grid.num_indices = index_i;
	mesh_grid.num_vertices = index_v;
	mesh_grid.draw = [=]() {
		glBindBuffer(GL_ARRAY_BUFFER, mesh_grid.vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_grid.index_buffer);
		attribfv("position", 2, 4, 0);
		attribfv("texel", 2, 4, 2);
		glDrawElements(GL_TRIANGLES, mesh_grid.num_indices, GL_UNSIGNED_INT, 0);
	};

	return true;
}

void free_game()
{
	glDeleteTextures(1, &tilemap);
}

void init_game()
{
	const int tiles[res_x * res_y] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 9, 10, 11,
		12, 13, 14, 15
	};
	
	tilemap = gen_texture(&tiles[0], res_x, res_y, GL_R32I, GL_RED_INTEGER, GL_INT, GL_NEAREST, GL_NEAREST);
}

void update_game(float dt)
{
	
}

void render_game(float dt)
{
	use_shader(shader_tilemap);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tilemap);
	uniform("tex", 0);
	uniform("tilemap", 1);
	uniform("tilesize", vec2i(15, 15));
	uniform("visible_res", vec2i(4, 4));
	uniform("texsize", vec2i(240, 2565));
	uniform("stride", 16);
	clearc(0x2a2a2aff);
	mesh_grid.draw();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }