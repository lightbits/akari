#include "game.h"
#include "sdlgui.h"

Shader 
	shader_cube,
	shader_blur,
	shader_ssao,
	shader_floor;

Mesh 
	mesh_cube;

mat4 
	mat_projection,
	mat_view;

int window_width,
	window_height;

const int num_cubes = 64;
vec3 cube_pos[num_cubes];
vec3 cube_vel[num_cubes];
vec3 cube_acc[num_cubes];

bool load_game(int width, int height)
{
	if (!shader_cube.load_from_file("./cube.vs", "./cube.fs") ||
		!shader_floor.load_from_file("./floor.vs", "./floor.fs"))
		return false;

	mesh_cube = Primitive::normal_cube();
	window_width = width;
	window_height = height;
    return true;
}

void free_game()
{
    
}

void reset_positions()
{
	for (int i = 0; i < num_cubes; i++)
	{
		float a = i / float(num_cubes);
		float x = snoise(31.0f * a + 17.0f);
		float y = snoise(17.0f * a + 511.0f);
		float z = snoise( 7.0f * a + 799.0f);
		cube_pos[i] = vec3(x, y, z);
		cube_vel[i] = snoise(71.0f * a + 1023.0f) * (vec3(x, y, z));
		cube_acc[i] = vec3(0.0f, 0.0f, 0.0f);
	}
}

void init_game()
{
	mat_projection = perspective(PI / 4.0f, window_width / float(window_height), 0.1f, 10.0f);
	reset_positions();
}

void update_game(float dt)
{
	
}

void render_game(float dt)
{
	static float damping_factor = 0.05f;
	static float spring_factor = 0.5f;
	static vec3 track_pos = vec3(0.0f);

	vec2 mouse_pos = get_mouse_pos();
	track_pos = 2.0f * vec3(mouse_pos.x / window_width, mouse_pos.y / window_height, 0.5f) - vec3(1.0f);

	float t = get_elapsed_time();
    mat_view = translate(0.0f, 0.0f, -4.0f) * rotateX(-0.3f) * rotateY(sin(t));
	
	for (int i = 0; i < num_cubes; i++)
	{
		cube_acc[i] = -spring_factor * (cube_pos[i] - track_pos) - damping_factor * cube_vel[i];
		cube_vel[i] += cube_acc[i] * dt;
		cube_pos[i] += cube_vel[i] * dt;
	}

	depth_test(true, GL_LEQUAL);
	depth_write(true);
	clear(0x000000ff, 1.0f);

	vec3 light_position = vec3(0.0f, 0.5f, 0.0f);
	vec3 light_color = vec3(1.0f, 0.9f, 0.87f);

	use_shader(shader_floor);
	uniform("projection", mat_projection);
	uniform("view", mat_view);
	uniform("model", translate(0.0f, -0.25f, 0.0f) * scale(10.0f, 0.05f, 10.0f));
	uniform("lightPosition", light_position);
	uniform("lightColor", light_color);
	mesh_cube.draw();

    use_shader(shader_cube);
	uniform("projection", mat_projection);
	uniform("view", mat_view);
	uniform("model", scale(1.0f));
	uniform("lightPosition", light_position);
	uniform("lightColor", light_color);

	for (int i = 0; i < num_cubes; i++)
	{
		uniform("model", translate(cube_pos[i]) * scale(0.1f));
		mesh_cube.draw();
	}

	ImGui::NewFrame();
	ImGui::Begin("Parameters");
	ImGui::SliderFloat("dampingFactor", &damping_factor, 0.0f, 2.0f);
	ImGui::SliderFloat("springFactor", &spring_factor, 0.0f, 10.0f);
	if (ImGui::Button("Reset positions"))
	{
		reset_positions();
	}
	ImGui::End();
	ImGui::Render();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }
