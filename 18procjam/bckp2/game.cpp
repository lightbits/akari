#include "game.h"

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

		error1 += max(y - 1.0f, 0.0f) * dt;
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
	float kp;
	float kd;
	float score;
	vector<float> trajectory;
};

float kp_min = 0.0f;
float kp_max = 10.0f;
float kd_min = 0.0f;
float kd_max = 10.0f;
int kp_n = 30;
int kd_n = 30;
int ikp = 15;
int ikd = 15;
int generation_size = 16;
vector<float> mutation_space_kp;
vector<float> mutation_space_kd;
vector< vector<float> > score_table;
vector<Mutation> generation;
float extinction_threshold = 0.5f; // If the score is > this, the species will become something new
float mutation_threshold = 0.1f; // If the score is > this, the species will mutate
float kp_mut_radius = 0.10f; // The maximum +- value when mutating kp
float kd_mut_radius = 0.05f; // The maximum +- value when mutating kd
uint tex;

bool load_game(int width, int height)
{
	if (!load_font(font, "../data/fonts/proggytinyttsz_8x12.png"))
		return false;

	gfx2d::init(width, height);
	gfx2d::use_font(font);

	return true;
}

void free_game()
{
	
}

void init_game()
{
	for (int i = 0; i < kp_n; i++)
		mutation_space_kp.push_back(kp_min + (kp_max - kp_min) * i / float(kp_n - 1));
	for (int i = 0; i < kd_n; i++)
		mutation_space_kd.push_back(kd_min + (kd_max - kd_min) * i / float(kd_n - 1));

	for (int i = 0; i < kp_n; i++)
	{
		vector<float> row;
		for (int j = 0; j < kd_n; j++)
			row.push_back(1.0f);
		score_table.push_back(row);
	}

	float kp = mutation_space_kp[ikp];
	float kd = mutation_space_kd[ikd];
	trajectory = calculate_trajectory(kp, kd, score);

	tex = gen_texture(NULL, kp_n, kd_n, GL_RG8, GL_RED, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
}

void update_game(float dt)
{
	if (is_key_down(SDLK_SPACE))
	{
		generation++;
		if (score > extinction_threshold)
		{
			ikp = int(frand() * (kp_n - 1));
			ikd	= int(frand() * (kd_n - 1));
		}
		else if (score > mutation_threshold)
		{
			ikp = ikp + kp_mut_radius * (-1.0f + 2.0f * frand()) * kp_n;
			ikd = ikd + kd_mut_radius * (-1.0f + 2.0f * frand()) * kd_n;
			ikp = clamp(ikp, 0, kp_n - 1);
			ikd = clamp(ikd, 0, kd_n - 1);
		}
		float kp = mutation_space_kp[ikp];
		float kd = mutation_space_kd[ikd];
		trajectory = calculate_trajectory(kp, kd, score);
		score_table[ikp][ikd] = score;
	}
}

void render_game(float dt)
{
	glBindTexture(GL_TEXTURE_2D, tex);
	float *pixels = new float[kp_n * kd_n * 2];
	for (int i = 0; i < kp_n; i++)
	{
		for (int j = 0; j < kd_n; j++)
		{
			pixels[2 * (j + i * kp_n) + 0] = score_table[i][j];
		}
	}
	pixels[2 * (ikd + ikp * kp_n) + 1] = 1.0f;
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, kp_n, kd_n, GL_RG, GL_FLOAT, pixels);
	delete[] pixels;

	gfx2d::begin();
	{
		using namespace gfx2d;
		blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
		clearc(0x44484Bff);

		float kp = mutation_space_kp[ikp];
		float kd = mutation_space_kd[ikd];
		Text text;
		text << "score: " << score << '\n';
		text << "kp: " << kp << '\n';
		text << "kd: " << kd << '\n';
		text << "generation: " << generation << '\n';

		draw_string(5.0f, 5.0f, text.getString());
		tex_rectangle(0.0f, 200.0f, 100.0f, 100.0f, tex);
		render_trajectory(trajectory, 470.0f, -240.0f, 5.0f, 320.0f);
	}
	gfx2d::end();
}