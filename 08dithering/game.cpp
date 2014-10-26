#include "game.h"

int 
	window_width,
	window_height;
Mesh 
	mesh_teapot,
	mesh_quad;
Shader 
	shader_default,
	shader_dither;
mat4
	mat_projection,
	mat_view;
uint
	tex_dither;
Camera
	camera;

RenderTexture rt_frame;

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;

	if (!shader_default.load_from_file("./default.vs", "./default.fs") ||
		!shader_dither.load_from_file("./dither.vs", "./dither.fs"))
		return false;

	//if (!load_mesh_indexed(mesh_teapot, "../../../resources/models/crytek-sponza/sponza.obj"))
	//	return false;
	if (!load_mesh_indexed(mesh_teapot, "../data/models/teapot.obj"))
		return false;

	uint8 dither_pattern[] = {
		255, 0, 255, 0
	};

	tex_dither = gen_texture(dither_pattern, 2, 2, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);

	mesh_quad = Primitive::tex_quad();
	return true;
}

void free_game()
{
	
}

void init_game()
{
	rt_frame = gen_rendertexture(window_width / 2, window_height / 2, GL_RGBA8);
	mat_view = mat4(1.0f);

	camera.setHorizontalAngle(-PI);
	camera.setVerticalAngle(0.0f);
	camera.setPosition(vec3(0.0f, 0.0f, -2.0f));
}

vec3 camera_pos = vec3(0.0f);
void update_game(float dt)
{
	if (is_key_down('a')) camera.moveLeft(dt);
	else if (is_key_down('d')) camera.moveRight(dt);
	if (is_key_down('w')) camera.moveForward(dt);
	else if (is_key_down('s')) camera.moveBackward(dt);
	if (is_key_down(SDLK_LCTRL)) camera.moveDown(dt);
	else if (is_key_down(SDLK_SPACE)) camera.moveUp(dt);

	if (is_mouse_down(MouseButtonLeft))
	{
		vec2i m = get_mouse_pos();
		float dx = (m.x - window_width / 2) / (float)window_width;
		float dy = (m.y - window_height / 2) / (float)window_height;
		float speed = 5.0f;
		camera.rotateHorizontal(speed * sqrt(abs(dx)) * dx * dt);
		camera.rotateVertical(speed * sqrt(abs(dy)) * dy * dt);
	}
	mat_view = camera.getViewMatrix();

	float t = get_elapsed_time();
	mat_projection = perspective(PI / 4.0f, window_width / float(window_height), 0.2f, 20.0f);
	//mat_view = translate(0.0f, -0.7f, -2.0f) * rotateX(-0.13f) * rotateY(t);
}

void render_game(float dt)
{
	glBindFramebuffer(GL_FRAMEBUFFER, rt_frame.fbo);
	glViewport(0, 0, rt_frame.width, rt_frame.height);
	glBindTexture(GL_TEXTURE_2D, tex_dither);

	depth_test(true, GL_LEQUAL);
	depth_write(true);
	clear(0x300224ff, 1.0f);

	use_shader(shader_default);
	uniform("projection", mat_projection);
	uniform("view", mat_view);
	uniform("model", scale(0.07f));
	uniform("lightPos", vec3(0.8, 4.0, 0.8));
	uniform("palette", vec3(143.0, 199.0, 132.0) / 255.0f);
	uniform("texDither", 0);
	//mesh_teapot.draw();
	glBindBuffer(GL_ARRAY_BUFFER, mesh_teapot.vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_teapot.index_buffer);
	attribfv("position", 3, 8, 0);
	attribfv("normal", 3, 8, 5);
	glDrawElements(GL_TRIANGLES, mesh_teapot.num_indices, GL_UNSIGNED_INT, 0);
	
	glBindTexture(GL_TEXTURE_2D, rt_frame.color);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window_width, window_height);
	clear(0x00000000, 1.0f);
	use_shader(shader_dither);
	uniform("texScene", 0);
	mesh_quad.draw();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }