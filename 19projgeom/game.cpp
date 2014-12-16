#include "game.h"
#define GLSL(src) "#version 150 core\n" #src

const char *TEST_VS = GLSL(
	in vec3 position;
	uniform mat4 projection;
	uniform mat4 view;
	uniform mat4 model;
	uniform vec3 color;

	void main()
	{
		vec4 wPosition = model * vec4(position, 1.0);
		vec4 vPosition = view * wPosition;
		if (wPosition.z > 1.0)
			vPosition.xy *= 1000.0;
		gl_Position = projection * vPosition;
	}
);

const char *TEST_FS = GLSL(
	uniform vec3 color;
	out vec4 out_color;
	void main()
	{
		out_color = vec4(color, 1.0);
	}
);

mat4 
	mat_projection,
	mat_view;
Mesh mesh_test;
Shader shader_test;
int 
	window_width,
	window_height;

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;
	if (!shader_test.load_from_src(TEST_VS, TEST_FS))
		return false;
	return true;
}

void free_game()
{
	
}

void init_game()
{
	mesh_test = Primitive::cube();
	mat_projection = perspective(PI / 4.0f, window_width / float(window_height), 0.1f, 10.0f);
}

void update_game(float dt)
{
	static float x, y;
	mat_view = translate(x, y, -3.0f);

	if (is_key_down('a'))
		x += dt;
	else if (is_key_down('d'))
		x -= dt;
	if (is_key_down('w'))
		y -= dt;
	else if (is_key_down('s'))
		y += dt;
}

void render_game(float dt)
{
	depth_write(true);
	depth_test(true);
	clear(0x2a2a2aff, 1.0f);
	use_shader(shader_test);
	uniform("projection", mat_projection);
	uniform("view", mat_view);

	uniform("color", vec3(1.0f, 0.4f, 0.3f));
	uniform("model", scale(0.25f, 0.25f, 0.5f) * translate(0.0f, 0.0f, 0.5f));
	mesh_test.draw();
	uniform("color", vec3(0.0f));
	uniform("model", translate(0.0f, 0.0f, 1.0f) * scale(0.23f, 0.21f, 1.0f) * rotateZ(0.1f));
	mesh_test.draw();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }