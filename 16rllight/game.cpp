#include "game.h"
#define GLSL(src) "#version 150 core\n" #src

static const char *GEOMETRY_VS = GLSL(
	in vec2 position;
	out vec2 texel;
	
	void main()
	{
		texel = 0.5 * position + vec2(0.5);
		gl_Position = vec4(position, 0.0, 1.0);
	}
);

static const char *GEOMETRY_FS = GLSL(
	in vec2 texel;
	uniform sampler2D tex;
	out vec4 outColor;

	void main()
	{
		outColor = texture(tex, texel);
	}
);

static const char *LIGHT_VS = GLSL(
	in vec2 position;
	uniform mat4 translation;
	uniform mat4 scale;
	uniform float aspect;
	out vec2 texel;
	
	void main()
	{
		texel = 0.5 * position + vec2(0.5);
		gl_Position = translation * scale * vec4(vec2(1.0, aspect) * position, 0.0, 1.0);
	}
);

static const char *LIGHT_FS = GLSL(
	in vec2 texel;
	out vec4 outColor;

	void main()
	{
		vec2 dt = 2.0 * texel - vec2(1.0);
		float r2 = dot(dt, dt);
		if (r2 > 1.0)
			discard;
		outColor = vec4(1.0);
	}
);

Shader 
	shader_geometry,
	shader_light;
Mesh quad;
uint 
	tex_fill,
	tex_line;
vec2 resolution;

bool load_game(int width, int height)
{
	if (!shader_geometry.load_from_src(GEOMETRY_VS, GEOMETRY_FS) ||
		!shader_light.load_from_src(LIGHT_VS, LIGHT_FS) ||
		!load_texture_from_file(tex_fill, "./test_fill.png") || 
		!load_texture_from_file(tex_line, "./test_line.png"))
		return false;
	resolution = vec2(width, height);
	return true;
}

void free_game()
{
	
}

void init_game()
{
	quad = Primitive::quad();
}

void update_game(float dt)
{
	
}

void stencil_light(float x, float y, float outer_radius, float inner_radius)
{
	uniform("translation", translate(x, y, 0.0f));

	glStencilFunc(GL_NEVER, 1, 0xFF);
	glStencilMask(0xFF);
	glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
	uniform("scale", scale(outer_radius));
	quad.draw();

	glStencilFunc(GL_NEVER, 2, 0xFF);
	uniform("scale", scale(inner_radius));
	quad.draw();
}

void render_game(float dt)
{
	vec2 p = 2.0f * vec2(get_mouse_pos()) / resolution - vec2(1.0f);

	glEnable(GL_STENCIL_TEST);
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);
	clearc(0x4a2a2a00);
	use_shader(shader_light);
	blend_mode(true, GL_ONE, GL_ONE, GL_FUNC_ADD);
	uniform("aspect", resolution.x / resolution.y);
	stencil_light(p.x, -p.y, 0.5f, 0.25f);

	use_shader(shader_geometry);
	uniform("tex", 0);
	blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glStencilMask(0x00);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glBindTexture(GL_TEXTURE_2D, tex_line);
	quad.draw();

	glStencilFunc(GL_EQUAL, 2, 0xFF);
	glBindTexture(GL_TEXTURE_2D, tex_fill);
	quad.draw();
	glDisable(GL_STENCIL_TEST);
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }