#include "game.h"

int window_width;
int window_height;

uint tex_sdf;
Mesh quad;
Shader shader_sdf;
float zoom = 1.0f;
mat4 
	projection,
	view;

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;

	//if (!load_texture_from_file(tex_sdf, "./input_test05.png", true))
	//	return false;

	if (!shader_sdf.load_from_file("./sdf.vs", "./sdf.fs"))
		return false;

	return true;
}

void free_game()
{
	shader_sdf.dispose();
	//glDeleteTextures(1, &tex_sdf);
}

void init_game()
{
	gfx2d::init(window_width, window_height);
	projection = perspective(PI / 4.0f, 1.0f, 0.1f, 10.0f);
	view = translate(0.0f, 0.0f, -3.0f) * rotateY(0.3f) * rotateX(-0.3f);
	quad = gen_tex_quad();
}

void update_game(float dt)
{
	if (is_key_down('z'))
		zoom += dt;
	else if (is_key_down('x'))
		zoom -= dt;
}

vec2 project(vec3 v)
{
	vec4 clip = projection * view * vec4(v, 1.0f);
	return vec2(window_width, window_height) * (0.5f * (vec2(clip.x, -clip.y) / clip.w) + vec2(0.5f));
}

void line3d(vec3 p0, vec3 p1, uint color)
{
	gfx2d::line(project(p0), project(p1), color);
}

void surf3d(vec3 p, vec3 n, float radius, uint color)
{
	vec3 u, r;
	if (abs(n.y) > 0.99f)
	{
		r = vec3(1.0f, 0.0f, 0.0f);
		u = normalize(cross(r, n));
		r = normalize(cross(n, u));
	}
	else
	{
		u = vec3(0.0f, 1.0f, 0.0f);
		r = normalize(cross(n, u));
		u = normalize(cross(r, n));
	}
	vec2 v0 = project(p - u * radius - r * radius);
	vec2 v1 = project(p - u * radius + r * radius);
	vec2 v2 = project(p + u * radius + r * radius);
	vec2 v3 = project(p + u * radius - r * radius);
	gfx2d::fill_quad(v0, v1, v2, v3, color);
}

void render_game(float dt)
{
	float t = get_elapsed_time();
	view = translate(0.0f, 0.0f, -3.0f) * rotateX(-0.5f) * rotateY(t * 0.2f);

	using namespace gfx2d;
	depth_test(true, GL_LEQUAL);
	depth_write(true);
	clear(0x2a2a2aff, 1.0f);
	//clearc(0x2a2a2aff);
	begin();
	line3d(vec3(0.0f), vec3(1.0f, 0.0f, 0.0f), 0xff8855ff);
	line3d(vec3(0.0f), vec3(0.0f, 1.0f, 0.0f), 0x88ff55ff);
	line3d(vec3(0.0f), vec3(0.0f, 0.0f, 1.0f), 0x5588ffff);

	//int n = 32;
	//for (int i = 0; i < n; i++)
	//{
	//	float t = PI * i / (2.0f * n);
	//	float rf = 0.5f + 0.5f * snoise(i * 5.0f);
	//	float tr = PI * (i + rf) / (2.0f * n);
	//	vec3 p = vec3(cos(t), sin(t), 0.0f);
	//	vec3 r = vec3(cos(tr), sin(tr), 0.0f);
	//	surf3d(p, p, PI / (2.0f * n), 0x5588ffff);
	//	line3d(vec3(0.0f), r, 0xff8855ff);
	//}

	int u_strats = 16;
	int v_strats = 16;
	int n_samples = u_strats * v_strats;
	for (int ui = 0; ui < u_strats; ui++)
	{
		for (int vi = 0; vi < v_strats; vi++)
		{
			float uf = noise2f(ui, vi);
			float vf = noise2f(ui * 3 + 255, vi * 5 + 127);
			float u = (ui + uf) * TWO_PI / (float)u_strats;
			float v = (vi + vf) * PI / (float)v_strats;
			float r = cos(v);
			float x = r * sin(u);
			float y = sin(v);
			float z = r * cos(u);

			vec3 p = vec3(x, y, z);
			uint8 red = 255 * (0.5f * x + 0.5f);
			uint8 gre = 255 * (0.5f * y + 0.5f);
			uint8 blu = 255 * (0.5f * z + 0.5f);
			float alpha = z * 0.5f + 0.5f;
			vec4 c = projection * view * vec4(x, y, z, 1.0f);
			set_layer(0.5f + 0.5f * c.z / c.w);
			surf3d(p, p, 0.025f, (red << 24) | (gre << 16) | (blu << 8) | 0xff);
		}
	}

	end();
	blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
	//clear(0x445788ff, 1.0);

	//use_shader(shader_sdf);
	//glBindTexture(GL_TEXTURE_2D, tex_sdf);
	//uniform("tex", 0);
	//uniform("zoom", zoom);
	//quad.draw();
}

void on_key_up(uint16 mod, SDL_Keycode key) 
{
	if (key == SDLK_r)
		load_game(window_width, window_height);
}

void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }