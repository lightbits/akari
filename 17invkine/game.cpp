/*
http://www.math.ucsd.edu/~sbuss/ResearchWeb/ikmethods/iksurvey.pdf
http://en.wikipedia.org/wiki/Inverse_kinematics
http://freespace.virgin.net/hugo.elias/models/m_ik2.htm
http://blog.oscarliang.net/inverse-kinematics-and-trigonometry-basics/
http://run.usc.edu/cs520-s14/ik/welman.pdf
http://www.learnaboutrobots.com/inverseKinematics.htm
http://the8bitpimp.wordpress.com/2014/10/28/paramatric-hexapod-animation-controller/
*/

#include "game.h"
#define GLSL(src) "#version 150 core\n" #src

static const char *GEOMETRY_VS = GLSL(
	in vec3 position;
	uniform mat4 projection;
	uniform mat4 view;
	uniform mat4 model;
	
	void main()
	{
		gl_Position = projection * view * model * vec4(position, 1.0);
	}
);

static const char *GEOMETRY_FS = GLSL(
	uniform vec3 color;
	out vec4 outColor;

	void main()
	{
		outColor = vec4(color, 1.0);
	}
);

Shader 
	shader_geometry;
Mesh 
	mesh_grid,
	mesh_box,
	mesh_line;
mat4
	mat_projection,
	mat_view;
int
	window_width,
	window_height;

Mesh gen_grid_box()
{
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		+0.5f, -0.5f, -0.5f,
		-0.5f, +0.5f, -0.5f,
		+0.5f, +0.5f, -0.5f,
		-0.5f, -0.5f, +0.5f,
		+0.5f, -0.5f, +0.5f,
		-0.5f, +0.5f, +0.5f,
		+0.5f, +0.5f, +0.5f,

		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, +0.5f,
		+0.5f, -0.5f, -0.5f,
		+0.5f, -0.5f, +0.5f,
		-0.5f, +0.5f, -0.5f,
		-0.5f, +0.5f, +0.5f,
		+0.5f, +0.5f, -0.5f,
		+0.5f, +0.5f, +0.5f,

		-0.5f, -0.5f, -0.5f,
		-0.5f, +0.5f, -0.5f,
		+0.5f, -0.5f, -0.5f,
		+0.5f, +0.5f, -0.5f,
		-0.5f, -0.5f, +0.5f,
		-0.5f, +0.5f, +0.5f,
		+0.5f, -0.5f, +0.5f,
		+0.5f, +0.5f, +0.5f,
	};

	Mesh mesh;
	mesh.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices);
	mesh.index_buffer = 0;
	mesh.num_indices = 0;
	mesh.num_vertices = 24;
	mesh.draw = [=]() {
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
		attribfv("position", 3, 3, 0);
		glDrawArrays(GL_LINES, 0, 24);
	};
	return mesh;
}

Mesh gen_line()
{
	float vertices[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	Mesh mesh;
	mesh.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices);
	mesh.index_buffer = 0;
	mesh.num_indices = 0;
	mesh.num_vertices = 2;
	mesh.draw = [=]() {
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
		attribfv("position", 3, 3, 0);
		glDrawArrays(GL_LINES, 0, 2);
	};
	return mesh;
}

bool load_game(int width, int height)
{
	if (!shader_geometry.load_from_src(GEOMETRY_VS, GEOMETRY_FS))
		return false;
	window_width = width;
	window_height = height;
	return true;
}

void free_game()
{
	
}

void init_game()
{
	mesh_grid = Primitive::grid(16);
	mesh_box = gen_grid_box();
	mesh_line = gen_line();
}

void spring_damper(vec3 &x, vec3 &dx, vec3 ref, float kp, float kd, float sigma, float dt)
{
	vec3 e = ref - x;
	vec3 a = kp * e - kd * dx;
	a += sigma * (vec3(-1.0f) + 2.0f * vec3(frand(), frand(), frand()));
	dx += a * dt;
	x += dx * dt;
}

void spring_damper(float &x, float &dx, float ref, float kp, float kd, float sigma, float dt)
{
	float e = ref - x;
	float a = kp * e - kd * dx;
	a += sigma * (-1.0f + 2.0f * frand());
	dx += a * dt;
	x += dx * dt;
}

void update_game(float dt)
{

	// Dynamic camera
	static vec3 up_direction = vec3(0.0f, 1.0f, 0.0f);
	static vec3 up_velocity = vec3(0.0f, 0.0f, 0.0f);
	static vec3 up_ref = vec3(0.0f, 1.0f, 0.0f);
	static vec3 position_ref = vec3(0.0f, 0.5f, 2.0f);
	static vec3 camera_position = vec3(0.0f, 0.0f, 0.0f);
	static vec3 camera_velocity = vec3(0.0f, 0.0f, 0.0f);
	static float zoom = PI / 4.0f;
	static float zoom_velocity = 0.0f;
	static float zoom_ref = PI / 4.0f;
	spring_damper(camera_position, camera_velocity, position_ref, 5.5f, 3.0f, 0.5f, dt);
	spring_damper(up_direction, up_velocity, up_ref, 0.5f, 0.25f, 0.25f, dt);
	spring_damper(zoom, zoom_velocity, zoom_ref, 5.5f, 3.0f, 0.1f, dt);
	up_direction = normalize(up_direction);
	mat_view = glm::lookAt(camera_position, vec3(0.0f, 0.5f, 0.0f), up_direction);
	mat_projection = perspective(zoom, window_width / float(window_height), 0.1f, 10.0f);

	if (is_key_down(SDLK_SPACE))
		mat_view = translate(0.0f, 0.0f, -4.0f) * rotateX(-0.34f) * rotateY(-0.5f);

	if (is_key_down('a'))
		position_ref.x -= dt;
	else if (is_key_down('d'))
		position_ref.x += dt;

	if (is_key_down('w'))
		position_ref.y -= dt;
	else if (is_key_down('s'))
		position_ref.y += dt;

	if (is_key_down('z'))
		zoom_ref -= 0.4f * dt;
	else if (is_key_down('x'))
		zoom_ref += 0.4f * dt;
}

void render_shadowed_mesh(const Mesh &mesh, const mat4 &model, const vec3 &color)
{
	uniform("model", scale(1.0f, 0.0f, 1.0f) * model);
	uniform("color", vec3(0.0f));
	mesh.draw();

	uniform("model", model);
	uniform("color", color);
	mesh.draw();
}

void render_grass(float dt)
{
	vec3 origin = vec3(0.0f);
	mat4 model = scale(1.0f, 0.25f, 0.0f);
	for (int i = 0; i < 4; i++)
	{
		uniform("model", rotateZ(i * 0.1f) * translate(0.0f, 0.25f * i, 0.0f) * scale(1.0f, 0.25f, 1.0f));
		uniform("color", vec3(1.0f));
		mesh_line.draw();
	}
}

void render_game(float dt)
{
	clearc(0x2a2a2aff);
	use_shader(shader_geometry);
	uniform("projection", mat_projection);
	uniform("view", mat_view);
	uniform("model", scale(1.0f));
	uniform("color", vec3(0.4f, 0.45f, 0.48f));
	mesh_grid.draw();

	float t = get_elapsed_time();
	render_shadowed_mesh(
		mesh_box, 
		translate(0.0f, 0.5f, 0.0f) * scale(0.25f, 0.25f, 1.0f), 
		vec3(1.0f));

	render_shadowed_mesh(
		mesh_line, 
		translate(0.0f, 0.5f, 0.0f) * scale(0.25f, 0.25f, 1.0f) * translate(-0.5f, +0.5f, -0.5f) * rotateZ(0.7f) * scale(1.0f, 0.5f, 1.0f), 
		vec3(1.0f));

	render_shadowed_mesh(
		mesh_line, 
		translate(0.0f, 0.5f, 0.0f) * scale(0.25f, 0.25f, 1.0f) * translate(-0.5f, +0.5f, +0.5f) * rotateZ(0.7f) * scale(1.0f, 0.5f, 1.0f), 
		vec3(1.0f));

	render_shadowed_mesh(
		mesh_line, 
		translate(0.0f, 0.5f, 0.0f) * scale(0.25f, 0.25f, 1.0f) * translate(+0.5f, +0.5f, -0.5f) * rotateZ(-0.7f) * scale(1.0f, 0.5f, 1.0f), 
		vec3(1.0f));

	render_shadowed_mesh(
		mesh_line, 
		translate(0.0f, 0.5f, 0.0f) * scale(0.25f, 0.25f, 1.0f) * translate(+0.5f, +0.5f, +0.5f) * rotateZ(-0.7f) * scale(1.0f, 0.5f, 1.0f), 
		vec3(1.0f));

	render_grass(dt);
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }