#include "game.h"
#include "sdlgui.h"

int 
	window_width,
	window_height;
Mesh 
	mesh_teapot,
	mesh_quad;
Shader 
	shader_texture,
	shader_depth,
	shader_scattering;
mat4
	mat_projection,
	mat_projection_light,
	mat_view_light,
	mat_view;

RenderTexture 
	rt_min_depth, 
	rt_max_depth;

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;

	if (!shader_texture.load_from_file("./texture.vs", "./texture.fs") ||
		!shader_depth.load_from_file("./depth.vs", "./depth.fs") ||
		!shader_scattering.load_from_file("./scattering.vs", "./scattering.fs"))
		return false;

	//if (!load_mesh_indexed(mesh_teapot, "../data/models/body.obj"))
	//	return false;

	if (!load_mesh_indexed(mesh_teapot, "../data/models/teapot.obj"))
		return false;
	mesh_teapot.draw = [=]() {
		glBindBuffer(GL_ARRAY_BUFFER, mesh_teapot.vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_teapot.index_buffer);
		attribfv("position", 3, 8, 0);
		attribfv("normal", 3, 8, 5);
		glDrawElements(GL_TRIANGLES, mesh_teapot.num_indices, GL_UNSIGNED_INT, 0);
	};

	mesh_quad = Primitive::tex_quad();
	return true;
}

void free_game()
{
	
}

void init_game()
{
	rt_min_depth = gen_rendertexture(window_width, window_height, GL_RG32F);
	rt_max_depth = gen_rendertexture(window_width, window_height, GL_RG32F);
	mat_view = mat4(1.0f);
	gfx2d::init(window_width, window_height);
}

void update_game(float dt)
{

}

void render_game(float dt)
{
	static float z_near = 0.1f;
	static float z_far = 6.0f;
	static float light_radius = 2.0f;
	static vec2 light_rotation = vec2(-0.43f, 0.0f);
	static vec2 view_rotation = vec2(-0.43f, 0.0f);
	static float model_scale = 0.07f;
	static float density = 2.0f;
	static float specular_intensity = 0.2f;
	static float specular_power = 4.0f;

	mat4 mat_model = scale(model_scale);
	mat_projection = perspective(PI / 4.0f, window_width / float(window_height), z_near, z_far);
	mat_view = translate(0.0f, 0.0f, -2.0f) * rotateX(view_rotation.x) * rotateY(view_rotation.y);
	mat_view_light = translate(0.0f, 0.0f, -light_radius) * rotateX(light_rotation.x) * rotateY(light_rotation.y);

	vec3 light_pos = vec3(glm::inverse(mat_view_light) * vec4(0.0f, 0.0f, 0.0f, 1.0f));

	// Render depth map
	use_shader(shader_depth);
	uniform("projection", mat_projection);
	uniform("view", mat_view);
	uniform("model", mat_model);
	mesh_teapot.draw();
	depth_test(false);
	depth_write(false);

	blend_mode(true, GL_ONE, GL_ONE, GL_MIN);
	glBindFramebuffer(GL_FRAMEBUFFER, rt_min_depth.fbo);
	glViewport(0, 0, rt_min_depth.width, rt_min_depth.height);
	clearc(0xff000000);
	glDrawElements(GL_TRIANGLES, mesh_teapot.num_indices, GL_UNSIGNED_INT, 0);

	blend_mode(true, GL_ONE, GL_ONE, GL_MAX);
	glBindFramebuffer(GL_FRAMEBUFFER, rt_max_depth.fbo);
	glViewport(0, 0, rt_max_depth.width, rt_max_depth.height);
	clearc(0x00000000);
	glDrawElements(GL_TRIANGLES, mesh_teapot.num_indices, GL_UNSIGNED_INT, 0);

	// Render the geometry
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window_width, window_height);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rt_min_depth.color);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, rt_max_depth.color);
	blend_mode(false);
	depth_test(true, GL_LEQUAL);
	depth_write(true);

	use_shader(shader_scattering);
	uniform("texMinDepth", 0);
	uniform("texMaxDepth", 1);
	uniform("zNear", z_near);
	uniform("zFar", z_far);
	uniform("density", density);
	uniform("specularPower", specular_power);
	uniform("specularIntensity", specular_intensity);
	uniform("eyePos", vec3(glm::inverse(mat_view) * vec4(0.0, 0.0, 0.0, 1.0)));
	uniform("lightPos", light_pos);
	uniform("lightMatrix", mat_projection * mat_view);
	uniform("projection", mat_projection);
	uniform("view", mat_view);
	uniform("model", mat_model);
	clear(0x00000000, 1.0f);
	mesh_teapot.draw();

	//use_shader(shader_texture);
	//uniform("texMinDepth", 0);
	//uniform("texMaxDepth", 1);
	//uniform("zNear", z_near);
	//uniform("zFar", z_far);
	//mesh_quad.draw();

	ImGui::NewFrame();
	ImGui::Begin("Scattering");
	ImGui::SliderFloat("modelScale", &model_scale, 0.01f, 1.0f);
	ImGui::SliderFloat("density", &density, 0.0f, 8.0f);
	ImGui::SliderFloat("zNear", &z_near, 0.1f, 4.0f);
	ImGui::SliderFloat("zFar", &z_far, 0.1f, 6.0f);
	ImGui::SliderFloat("lightRadius", &light_radius, 0.0f, 4.0f);
	ImGui::SliderAngle("lightRotateX", &light_rotation[0]);
	ImGui::SliderAngle("lightRotateY", &light_rotation[1]);
	ImGui::SliderAngle("viewRotateX", &view_rotation[0]);
	ImGui::SliderAngle("viewRotateY", &view_rotation[1]);
	ImGui::SliderFloat("specularPower", &specular_power, 1.0f, 64.0f);
	ImGui::SliderFloat("specularIntensity", &specular_intensity, 0.0f, 1.0f);
	ImGui::End();
	ImGui::Render();

	gfx2d::begin();
	vec4 lp_c = mat_projection * mat_view * vec4(light_pos, 1.0f);
	lp_c = vec4(0.5f) + 0.5f * lp_c / lp_c.w;
	vec2 texel = vec2(lp_c.x, 1.0f - lp_c.y);
	texel *= vec2(window_width, window_height);
	gfx2d::fill_rectangle(texel.x - 8.0f, texel.y - 8.0f, 16.0f, 16.0f, 0x8899ffff);
	gfx2d::end();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }