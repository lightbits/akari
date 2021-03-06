#include "gfx2d.h"
#include "opengl.h"
#include "gfx.h"
#include "log.h"
#define GLSL(src) "#version 150 core\n" #src

static const char *VERTEX_SHADER_SRC = GLSL(
	in vec3 position;
	in vec2 texel;
	in vec4 color;
	out vec4 vColor;
	out vec2 vTexel;
	out float vDepth;
	uniform mat4 projection;

	void main()
	{
		vColor = color;
		vTexel = texel;
		vDepth = position.z;
		gl_Position = projection * vec4(floor(position.xy + vec2(0.375, 0.375)), 0.0, 1.0);
	}
);

static const char *FRAGMENT_SHADER_SRC = GLSL(
	in vec4 vColor;
	in vec2 vTexel;
	in float vDepth;
	uniform sampler2D tex;
	out vec4 outColor;

	void main()
	{
		outColor = texture(tex, vTexel) * vColor;
		gl_FragDepth = vDepth;
	}
);

struct DrawCmd
{
	uint texture;
	int count;
	GLenum mode;
};

static vector<DrawCmd> draw_cmds;
static const int MAX_VERTICES = 2048;
static const int VERTEX_SIZE = 9 * sizeof(float);
static int vertex_count = 0;

static uint vertex_buffer;
static uint blank_texture;
static Font *current_font = nullptr;
static Shader shader;
static mat4 mat_ortho;
static float layer = 0.0f;
static bool in_begin_end_pair = false;

void gfx2d::init(int width, int height)
{
	uint8 blank[] = { 255, 255, 255, 255 };
	blank_texture = gen_texture(blank, 1, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

	vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, MAX_VERTICES * VERTEX_SIZE, NULL, GL_DYNAMIC_DRAW);

	bool status = shader.load_from_src(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC);
	ASSERT(status, "Failed to load default 2D shader");

	mat_ortho = ortho(0.0f, float(width), float(height), 0.0f, 0.0f, 1.0f);
	draw_cmds.clear();
}

void gfx2d::dispose()
{
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteTextures(1, &blank_texture);
	current_font = nullptr;
	shader.dispose();
	draw_cmds.clear();
}

void flush()
{
	if (draw_cmds.size() == 0)
		return;

	GLenum current_mode = draw_cmds[0].mode;
	uint current_texture = draw_cmds[0].texture;
	glBindTexture(GL_TEXTURE_2D, current_texture);
	int begin = 0;
	int count = 0;
	for (int i = 0; i < draw_cmds.size(); i++)
	{
		GLenum mode = draw_cmds[i].mode;
		uint texture = draw_cmds[i].texture;
		if (current_mode != mode || current_texture != texture || current_mode == GL_LINE_STRIP)
		{
			glBindTexture(GL_TEXTURE_2D, current_texture);
			glDrawArrays(current_mode, begin, count);
			begin += count;
			count = 0;
			current_mode = mode;
			current_texture = texture;
		}
		count += draw_cmds[i].count;
	}
	if (count > 0)
	{
		glBindTexture(GL_TEXTURE_2D, current_texture);
		glDrawArrays(current_mode, begin, count);
	}

	vertex_count = 0;
	draw_cmds.clear();
}

void gfx2d::begin()
{
	ASSERT(!in_begin_end_pair, "You need to end() before you start a new begin()");
	in_begin_end_pair = true;

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glActiveTexture(GL_TEXTURE0);
	use_shader(shader);
	attribfv("position", 3, 9, 0);
	attribfv("texel", 2, 9, 3);
	attribfv("color", 4, 9, 5);
	uniform("projection", mat_ortho);
	uniform("tex", 0);
}

void gfx2d::end()
{
	ASSERT(in_begin_end_pair, "You need to begin() before you end()");
	in_begin_end_pair = false;

	flush();
	unset_attrib("position");
	unset_attrib("texel");
	unset_attrib("color");
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void append_data(float *data, int count, GLenum mode, uint texture)
{
	if (vertex_count + count > MAX_VERTICES)
		flush();

	DrawCmd cmd;
	cmd.count = count;
	cmd.mode = mode;
	cmd.texture = texture;
	draw_cmds.push_back(cmd);
	glBufferSubData(GL_ARRAY_BUFFER, vertex_count * VERTEX_SIZE, count * VERTEX_SIZE, data);
	vertex_count += count;
}

void gfx2d::set_layer(float depth)
{
	layer = depth;
}

void gfx2d::draw_line(vec2 p0, vec2 p1, uint color) { draw_line(p0.x, p0.y, p1.x, p1.y, color); }
void gfx2d::draw_line(float x0, float y0, float x1, float y1, uint color)
{
	vec4 v_color = to_rgba(color);
	float data[] = {
		x0, y0, layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x1, y1, layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w
	};
	append_data(data, 2, GL_LINES, blank_texture);
}

void gfx2d::draw_rectangle(vec2 p, vec2 size, uint color) { draw_rectangle(p.x, p.y, size.x, size.y, color); }
void gfx2d::draw_rectangle(float x, float y, float w, float h, uint color)
{
	vec4 v_color = to_rgba(color);
	float data[] = {
		x, y,			layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x + w, y,		layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x + w, y + h,	layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x, y + h,		layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x, y,			layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w
	};
	append_data(data, 5, GL_LINE_STRIP, blank_texture);
}

void gfx2d::draw_fill_quad(vec2 v0, vec2 v1, vec2 v2, vec2 v3, uint color)
{
	vec4 v_color = to_rgba(color);
	float data[] = {
		v0.x, v0.y,	layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		v1.x, v1.y,	layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		v2.x, v2.y,	layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		v2.x, v2.y,	layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		v3.x, v3.y,	layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		v0.x, v0.y,	layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w
	};
	append_data(data, 6, GL_TRIANGLES, blank_texture);
}

void gfx2d::draw_fill_rectangle(vec2 p, vec2 size, uint color) { draw_fill_rectangle(p.x, p.y, size.x, size.y, color); }
void gfx2d::draw_fill_rectangle(float x, float y, float w, float h, uint color)
{
	vec4 v_color = to_rgba(color);
	float data[] = {
		x, y,			layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x + w, y,		layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x + w, y + h,	layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x + w, y + h,	layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x, y + h,		layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x, y,			layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w
	};
	append_data(data, 6, GL_TRIANGLES, blank_texture);
}

void gfx2d::draw_tex_rectangle(vec2 p, vec2 size, uint texture, uint color) { draw_tex_rectangle(p.x, p.y, size.x, size.y, texture, color); }
void gfx2d::draw_tex_rectangle(float x, float y, float w, float h, uint texture, uint color)
{
	vec4 v_color = to_rgba(color);
	float data[] = {
		x, y,			layer, 0.0f, 1.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x + w, y,		layer, 1.0f, 1.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x + w, y + h,	layer, 1.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x + w, y + h,	layer, 1.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x, y + h,		layer, 0.0f, 0.0f, v_color.x, v_color.y, v_color.z, v_color.w,
		x, y,			layer, 0.0f, 1.0f, v_color.x, v_color.y, v_color.z, v_color.w
	};
	append_data(data, 6, GL_TRIANGLES, texture);
}

void gfx2d::use_font(Font &font)
{
	current_font = &font;
}

vec2i gfx2d::measure_string(const string &text)
{
	int height = current_font->char_height;
	int width = 0;
	int curr_width = 0;
	for (int i = 0; i < text.size(); i++)
	{
		char c = text[i];
		Glyph glyph = current_font->glyphs.find(c)->second;
		if (c == '\n')
		{
			height += current_font->char_height;
			curr_width = 0;
		}
		curr_width += glyph.width;
		if (curr_width > width)
			width = curr_width;
	}

	return vec2i(width, height);
}

void gfx2d::draw_string(vec2 p, const string &text, uint color, bool centered, float sx, float sy) { draw_string(p.x, p.y, text, color, centered, sx, sy); }
void gfx2d::draw_string(float x0, float y0, const string &text, uint color, bool centered, float sx, float sy)
{
	// Nothing to draw
	if (text.size() == 0)
		return;

	SDL_assert(current_font != nullptr);
	if (current_font == nullptr)
		return;

	if (centered)
	{
		vec2i size = measure_string(text);
		x0 -= size.x * sx / 2.0f;
		y0 -= size.y * sy / 2.0f;
	}

	x0 = round(x0);
	y0 = round(y0);

	vec4 rgba = to_rgba(color);

	vector<float> v_buffer;

	float x = x0;
	float y = y0;
	float line_height = current_font->char_height * sy;
	for (int j = 0; j < text.size(); j++)
	{
		char c = text[j];
		if (c == '\n')
		{
			y += line_height;
			x = x0;
			continue;
		}

		Glyph glyph = current_font->glyphs.find(c)->second;
		float w = float(glyph.width) * sx;
		float h = float(glyph.height) * sy;
		float quad[] = {
			x, y,			layer, glyph.u_left,  glyph.v_bottom,	rgba.r, rgba.g, rgba.b, rgba.a,
			x + w, y,		layer, glyph.u_right, glyph.v_bottom,	rgba.r, rgba.g, rgba.b, rgba.a,
			x + w, y + h,	layer, glyph.u_right, glyph.v_top,		rgba.r, rgba.g, rgba.b, rgba.a,
			x + w, y + h,	layer, glyph.u_right, glyph.v_top,		rgba.r, rgba.g, rgba.b, rgba.a,
			x, y + h,		layer, glyph.u_left,  glyph.v_top,		rgba.r, rgba.g, rgba.b, rgba.a,
			x, y,			layer, glyph.u_left,  glyph.v_bottom,	rgba.r, rgba.g, rgba.b, rgba.a
		};

		v_buffer.insert(v_buffer.end(), quad, quad + sizeof(quad) / sizeof(float));

		x += w;
	}

	append_data(&v_buffer[0], text.size() * 6, GL_TRIANGLES, current_font->texture);
}