#include "game.h"
#include "sdlgui.h"

int
	window_width,
	window_height;

Mesh mesh_terrain, mesh_quad;
RenderTexture rt_heightmap;
mat4 mat_projection;
Shader shader_terrain, shader_generate;

float 
	terrain_frequency,
	terrain_scale,
	terrain_height;
int 
	terrain_samples;

Mesh gen_terrain_mesh(int samples_x, int samples_y)
{
	vector<vec3> vertices;
	vector<uint32> indices;
	int i = 0;
	for (int y = 0; y < samples_y; y++)
	{
		for (int x = 0; x < samples_x; x++)
		{
			float x0 = -1.0f + 2.0f * x / (samples_x - 1);
			float y0 = -1.0f + 2.0f * y / (samples_y - 1);
			float x1 = -1.0f + 2.0f * (x + 1) / (samples_x - 1);
			float y1 = -1.0f + 2.0f * (y + 1) / (samples_y - 1);
			vertices.push_back(vec3(x0, 0.0f, y0));
			vertices.push_back(vec3(x1, 0.0f, y0));
			vertices.push_back(vec3(x1, 0.0f, y1));
			vertices.push_back(vec3(x0, 0.0f, y1));
			indices.push_back(i + 0);
			indices.push_back(i + 1);
			indices.push_back(i + 2);
			indices.push_back(i + 2);
			indices.push_back(i + 3);
			indices.push_back(i + 0);
			i += 4;
		}
	}
	Mesh mesh;
	mesh.num_indices = indices.size();
	mesh.num_vertices = vertices.size();
	mesh.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
	mesh.index_buffer = gen_buffer(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32), &indices[0], GL_STATIC_DRAW);
	mesh.draw = [=]() {
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffer);
		attribfv("position", 3, 3, 0);
		glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, 0);
	};
	return mesh;
}

void gen_heightmap()
{
	use_shader(shader_generate);
	glBindFramebuffer(GL_FRAMEBUFFER, rt_heightmap.fbo);
	glViewport(0, 0, rt_heightmap.width, rt_heightmap.height);
	mesh_quad.draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window_width, window_height);
}

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;

	if (!shader_terrain.load_from_file("./terrain.vs", "./terrain.fs") ||
		!shader_generate.load_from_file("./generate.vs", "./generate.fs"))
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
	mesh_terrain = gen_terrain_mesh(32, 32);
	mesh_quad = gen_quad();
	rt_heightmap = gen_rendertexture(32, 32, GL_RGBA32F, GL_LINEAR, GL_LINEAR);
	gen_heightmap();
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
	//glBindTexture(GL_TEXTURE_2D, rt_heightmap.color);
	//uniform("heightmap", 0);
	uniform("projection", mat_projection);
	uniform("view", mat_view);
	uniform("model", scale(1.0f));
	uniform("lightPos", light_pos);
	uniform("lightColor", light_color);
	uniform("fogColor", fog_color);
	uniform("fogDensity", fog_density);
	uniform("ambient", ambient);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	mesh_terrain.draw();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
	ImGui::End();
	ImGui::Render();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }