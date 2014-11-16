#include "game.h"
#include <algorithm>

Font font;

vector<float> calculate_trajectory(float kp, float kd, float &score)
{
	float y0 = 0.0f;
	float v0 = 0.0f;
	float t = 0.0f;
	float dt = 0.01f;
	float T = 10.0f;
	float y = y0;
	float v = v0;
	vector<float> trajectory;
	float error1 = 0.0f;
	float error2 = 0.0f;
	while (t < T)
	{
		float a = -kp * (y - 1.0f) - kd * v;
		v += a * dt;
		y += v * dt;

		error1 -= max(y - 1.0f, 0.0f) * dt;
		error2 += (y - 1.0f) * (y - 1.0f) * dt;

		t += dt;
		trajectory.push_back(y);
	}
	score = 10.0f * error1 + error2;
	return trajectory;
}

void render_trajectory(vector<float> &trajectory, float sx, float sy, float x, float y)
{
	using namespace gfx2d;
	int n = trajectory.size();
	int step = n / 32;
	float dx = 1.0f / float(n - 1);
	float x0 = 0.0f;
	float y0 = trajectory[0];
	int i = step;
	while (i < n)
	{
		float y1 = trajectory[i];
		float x1 = i * dx;
		line(x + x0 * sx, y + y0 * sy, x + x1 * sx, y + y1 * sy, 0xffffffff);
		x0 = x1;
		y0 = y1;
		i += step;
	}
}

struct Mutation
{
	int kpi;
	int kdi;
	float score;
	vector<float> trajectory;
};

vector<float> gen_range(float min_value, float max_value, int n)
{
	vector<float> range;
	for (int i = 0; i < n; i++)
		range.push_back(min_value + (max_value - min_value) * i / float(n - 1));
	return range;
}

const int KP_N = 30;
const int KD_N = 30;
const int GENERATION_SIZE = 16;
const float KP_MIN = 0.01f;
const float KP_MAX = 10.0f;
const float KD_MIN = 0.01f;
const float KD_MAX = 10.0f;
vector< vector<float> > score_table;
vector<float> range_kp;
vector<float> range_kd;
vector<Mutation> generation;
int window_width;
int window_height;
uint tex;

bool load_game(int width, int height)
{
	if (!load_font(font, "../data/fonts/proggytinyttsz_8x12.png"))
		return false;

	window_width = width;
	window_height = height;
	gfx2d::init(width, height);
	gfx2d::use_font(font);

	return true;
}

void free_game()
{
	
}

Mutation gen_mutation(int kpi, int kdi)
{
	Mutation mut;
	mut.kpi = kpi;
	mut.kdi = kdi;
	mut.trajectory = calculate_trajectory(range_kp[kpi], range_kd[kdi], mut.score);
	return mut;
}

void init_game()
{
	range_kp = gen_range(KP_MIN, KP_MAX, KP_N);
	range_kd = gen_range(KD_MIN, KD_MAX, KD_N);

	for (int i = 0; i < KP_N; i++)
	{
		vector<float> row;
		for (int j = 0; j < KD_N; j++)
			row.push_back(9999.0f);
		score_table.push_back(row);
	}

	for (int i = 0; i < GENERATION_SIZE; i++)
	{
		int kpi = int(frand() * (KP_N - 1));
		int kdi = int(frand() * (KP_N - 1));
		generation.push_back(gen_mutation(kpi, kdi));
	}

	tex = gen_texture(NULL, KP_N, KD_N, GL_RG8, GL_RED, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
}

bool space_down = false;
void update_game(float dt)
{
	if (is_key_down(SDLK_SPACE) && !space_down)
	{
		space_down = true;
		const int k = 4;
		const float k_radius = 4.0f;
		vector<Mutation> next_gen;

		int best = 0;
		for (int i = 0; i < GENERATION_SIZE; i++)
			if (generation[i].score < generation[best].score)
				best = i;

		for (int i = 0; i < k; i++)
		{
			int offset_kpi = int((-1.0f + 2.0f * frand()) * k_radius);
			int offset_kdi = int((-1.0f + 2.0f * frand()) * k_radius);
			int kpi = generation[best].kpi + offset_kpi;
			int kdi = generation[best].kdi + offset_kdi;
			kpi = clamp(kpi, 0, KP_N - 1);
			kdi = clamp(kdi, 0, KD_N - 1);
			Mutation mut = gen_mutation(kpi, kdi);
			score_table[kpi][kdi] = mut.score;
			next_gen.push_back(mut);
		}

		for (int i = 0; i < GENERATION_SIZE - k; i++)
		{
			float r = frand();
			int kpi = generation[i].kpi;
			int kdi = generation[i].kdi;
			float score = generation[i].score;
			int step = score * score * 10.0f + 5.0f * score; // This is hard to tune... How to do this?
			if (r < 0.25)
				kpi += step;
			else if (r < 0.5)
				kpi -= step;
			else if (r < 0.75)
				kdi += step;
			else if (r < 1.00)
				kdi -= step;

			kpi = clamp(kpi, 0, KP_N - 1);
			kdi = clamp(kdi, 0, KD_N - 1);
			Mutation mut = gen_mutation(kpi, kdi);
			score_table[kpi][kdi] = mut.score;
			next_gen.push_back(mut);
		}

		generation = next_gen;
	}
	else if (!is_key_down(SDLK_SPACE))
	{
		space_down = false;
	}
}

void render_game(float dt)
{
	glBindTexture(GL_TEXTURE_2D, tex);
	float *pixels = new float[KP_N * KD_N * 2];
	for (int i = 0; i < KP_N; i++)
	{
		for (int j = 0; j < KD_N; j++)
		{
			pixels[2 * (j + i * KP_N) + 0] = score_table[i][j];
		}
	}
	for (int i = 0; i < GENERATION_SIZE; i++)
		pixels[2 * (generation[i].kdi + generation[i].kpi * KP_N) + 1] = 1.0f;
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, KP_N, KD_N, GL_RG, GL_FLOAT, pixels);
	delete[] pixels;

	gfx2d::begin();
	{
		using namespace gfx2d;
		blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
		clearc(0x44484Bff);

		for (int i = 0; i < generation.size(); i++)
		{
			float sx = window_width / 4.0f;
			float sy = window_height / 8.0f;
			float x = (i % 4) * sx;
			float y = (i / 4) * window_height / 4.0f + window_height / 6.0f;
			Text text;
			text << generation[i].score;
			draw_string(x, y, text.getString());
			render_trajectory(generation[i].trajectory, sx, -sy, x, y);
		}

		if (is_key_down('t'))
			tex_rectangle(0.0f, 0.0f, window_width, window_height, tex);
	}
	gfx2d::end();
}