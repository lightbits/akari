#include "game.h"
#include "sdlgui.h"

int
	window_width,
	window_height;

Mesh 
	mesh_terrain;

mat4 
	mat_projection;

Shader
	shader_terrain;

float 
	terrain_frequency,
	terrain_scale,
	terrain_height;
int 
	terrain_samples;

Mesh gen_terrain(float frequency, int samples, float scale, float height)
{
	float ds = frequency / samples;
	vector<float> heights(samples * samples);
	for (int i = 0; i < heights.size(); i++)
	{
		int xi = i % samples;
		int yi = i / samples;
		float xf = (xi / float(samples)) * 2.0f - 1.0f;
		float yf = (yi / float(samples)) * 2.0f - 1.0f;
		float rsqrd = xf * xf + yf * yf;
		float h0 = fBm(xi * ds, yi * ds);
		float h1 = 1.5f * exp(-rsqrd * 1.3f);
		heights[i] = h0 * h1 * height;
	}

	struct vertex
	{
		vertex() { }
		vertex(vec3 P, vec3 N) : position(P), normal(N) { }
		vec3 position;
		vec3 normal;
	};
	vector<vertex> vertices;
	for(int y = 0; y < samples - 1; ++y)
	{
		for(int x = 0; x < samples - 1; ++x)
		{
			int i00 = y * samples + x;
			int i10 = y * samples + x + 1;
			int i01 = (y + 1) * samples + x;
			int i11 = (y + 1) * samples + x + 1;

			// Model-scale coordinates
			float xf0 = scale * ((x + 0) / float(samples - 1) - 0.5f);
			float xf1 = scale * ((x + 1) / float(samples - 1) - 0.5f);
			float zf0 = scale * ((y + 0) / float(samples - 1) - 0.5f);
			float zf1 = scale * ((y + 1) / float(samples - 1) - 0.5f);

			vec3 v00 = vec3(xf0, heights[i00], zf0); // top-left
			vec3 v10 = vec3(xf1, heights[i10], zf0); // top-right
			vec3 v01 = vec3(xf0, heights[i01], zf1); // bottom-left
			vec3 v11 = vec3(xf1, heights[i11], zf1); // bottom-right
			vec3 n0 = normalize(cross(v00 - v10, v11 - v10));
			vec3 n1 = normalize(cross(v11 - v01, v00 - v01));

			vertices.push_back(vertex(v00, n0));
			vertices.push_back(vertex(v10, n0));
			vertices.push_back(vertex(v11, n0));
			vertices.push_back(vertex(v11, n1));
			vertices.push_back(vertex(v01, n1));
			vertices.push_back(vertex(v00, n1));
		}
	}

	Mesh mesh;
	mesh.num_vertices = vertices.size();
	mesh.num_indices = 0;
	mesh.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);
	mesh.index_buffer = 0;
	mesh.draw = [=]() {
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
		attribfv("position", 3, 6, 0);
		attribfv("normal", 3, 6, 3);
		glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices);
	};
	return mesh;
}

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;

	if (!shader_terrain.load_from_file("./terrain.vs", "./terrain.fs"))
		return false;

	return true;
}

void free_game()
{

}

void init_game()
{
	mat_projection = perspective(PI / 4.0f, window_width / float(window_height), 0.1f, 10.0f);

	terrain_frequency = 10.28f;
	terrain_samples = 32;
	terrain_scale = 7.978f;
	terrain_height = 0.6f;
	mesh_terrain = gen_terrain(terrain_frequency, terrain_samples, terrain_scale, terrain_height);
}

void update_game(float dt)
{

}

void render_game(float dt)
{
	static vec3 light_pos = vec3(0.0f, 2.0f, 2.0f);
	static vec3 light_color = vec3(1.0f, 0.8f, 0.5f);
	static vec3 ambient = vec3(67.0f, 66.0f, 63.0f) / 255.0f;
	static vec3 background = vec3(0.55, 0.45, 0.45);
	static vec2 view_rot = vec2(-0.3f, 2.76f);
	static float view_radius = 2.0f;
	static float fog_density = 0.206f;
	static vec3 fog_color = background * background;
	mat4 mat_view = translate(0.0f, 0.0f, -view_radius) * rotateX(view_rot.x) * rotateY(view_rot.y);
	depth_test(true, GL_LEQUAL);
	depth_write(true);
	clear(vec4(background, 1.0f), 1.0f);
	use_shader(shader_terrain);
	uniform("projection", mat_projection);
	uniform("view", mat_view);
	uniform("model", scale(1.0f));
	uniform("lightPos", light_pos);
	uniform("lightColor", light_color);
	uniform("fogColor", fog_color);
	uniform("fogDensity", fog_density);
	uniform("ambient", ambient);
	mesh_terrain.draw();

	ImGui::NewFrame();
	ImGui::Begin("Terrain shader");
	ImGui::SliderFloat3("lightPos", &light_pos[0], -2.0f, 2.0f);
	ImGui::ColorEdit3("lightColor", &light_color[0]);
	ImGui::ColorEdit3("fogColor", &fog_color[0]);
	ImGui::SliderFloat("fogDensity", &fog_density, 0.0f, 1.0f);
	ImGui::ColorEdit4("background", &background[0]);
	ImGui::SliderAngle("viewRotX", &view_rot[0]);
	ImGui::SliderAngle("viewRotY", &view_rot[1]);
	ImGui::SliderFloat("terrainScale", &terrain_scale, 1.0f, 16.0f);
	ImGui::SliderFloat("terrainFrequency", &terrain_frequency, 0.5f, 3.0f * terrain_scale);
	ImGui::SliderFloat("terrainHeight", &terrain_height, 0.0f, 2.0f);
	ImGui::SliderInt("terrainSamples", &terrain_samples, 1, 128);
	ImGui::SliderFloat("viewRadius", &view_radius, 0.0f, 4.0f);
	if (ImGui::Button("Reload terrain"))
	{
		mesh_terrain.dispose();
		mesh_terrain = gen_terrain(terrain_frequency, terrain_samples, terrain_scale, terrain_height);
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