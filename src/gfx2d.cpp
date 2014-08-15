#include "gfx2d.h"
#include "opengl.h"
#include "gfx.h"

struct DrawCmd
{
	int count;
	GLenum mode;
};

static vector<DrawCmd> draw_cmds;
static int max_vertices = 1024;
static int vertex_count = 0;
static uint vertex_buffer;
static Shader shader;
static int vertex_size = 6 * sizeof(float);
static mat4 mat_ortho;

void gfx2d::init(int width, int height)
{
	vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, max_vertices * vertex_size, NULL, GL_DYNAMIC_DRAW);
	SDL_assert(shader.load_from_file("./rectangle.vs", "./rectangle.fs"));
	draw_cmds.clear();
	mat_ortho = ortho(0.0f, float(width), float(height), 0.0f, 0.0f, 1.0f);
}

void flush()
{
	if (draw_cmds.size() == 0)
		return;

	GLenum current_mode = draw_cmds[0].mode;
	int begin = 0;
	int count = 0;
	for (int i = 0; i < draw_cmds.size(); i++)
	{
		GLenum mode = draw_cmds[i].mode;
		if (current_mode != mode)
		{
			glDrawArrays(current_mode, begin, count);
			begin += count;
			count = 0;
			current_mode = mode;
		}
		count += draw_cmds[i].count;
	}
	if (count > 0)
		glDrawArrays(current_mode, begin, count);

	draw_cmds.clear();
}

void gfx2d::begin()
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	use_shader(shader);
	attribfv("position", 2, 6, 0);
	attribfv("color", 4, 6, 2);
	uniform("projection", mat_ortho);
}

void gfx2d::end()
{
	flush();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	vertex_count = 0;
}

void append_data(float *data, int count, GLenum mode)
{
	if (vertex_count + count > max_vertices)
		flush();

	DrawCmd cmd;
	cmd.count = count;
	cmd.mode = mode;
	draw_cmds.push_back(cmd);
	glBufferSubData(GL_ARRAY_BUFFER, vertex_count * vertex_size, count * vertex_size, data);
	vertex_count += count;
}

void gfx2d::line(float x0, float y0, float x1, float y1, uint color)
{
	vec4 v_color = to_rgba(color);
	float data[] = {
		x0, y0, v_color.x, v_color.y, v_color.z, v_color.w,
		x1, y1, v_color.x, v_color.y, v_color.z, v_color.w
	};
	append_data(data, 2, GL_LINES);
}

void gfx2d::rectangle(float x, float y, float w, float h, uint color)
{
	vec4 v_color = to_rgba(color);
	float data[] = {
		x, y, v_color.x, v_color.y, v_color.z, v_color.w,
		x + w, y, v_color.x, v_color.y, v_color.z, v_color.w,
		x + w, y + h, v_color.x, v_color.y, v_color.z, v_color.w,
		x, y + h, v_color.x, v_color.y, v_color.z, v_color.w,
		x, y, v_color.x, v_color.y, v_color.z, v_color.w
	};
	append_data(data, 5, GL_LINE_STRIP);
}

void gfx2d::fill_rectangle(float x, float y, float w, float h, uint color)
{
	vec4 v_color = to_rgba(color);
	float data[] = {
		x, y, v_color.x, v_color.y, v_color.z, v_color.w,
		x + w, y, v_color.x, v_color.y, v_color.z, v_color.w,
		x + w, y + h, v_color.x, v_color.y, v_color.z, v_color.w,
		x + w, y + h, v_color.x, v_color.y, v_color.z, v_color.w,
		x, y + h, v_color.x, v_color.y, v_color.z, v_color.w,
		x, y, v_color.x, v_color.y, v_color.z, v_color.w
	};
	append_data(data, 6, GL_TRIANGLES);
}