#include "game.h"
#include "imgui.h"

uint
	tex_font,
	tex_test;

Shader
	shader_tex;

Mesh
	quad;

int
	window_width,
	window_height;

void render_gui(ImDrawList** const cmd_lists, int cmd_lists_count)
{
	size_t total_vtx_count = 0;
	for (int n = 0; n < cmd_lists_count; n++)
		total_vtx_count += cmd_lists[n]->vtx_buffer.size();
	if (total_vtx_count == 0)
		return;

	int read_pos_clip_rect_buf = 0;

	ImVector<ImVec4> clip_rect_stack;
	clip_rect_stack.push_back(ImVec4(-9999,-9999,+9999,+9999));

	mat4 mat_projection = ortho(0.0f, float(window_width), 0.0f, float(window_height), 0.0f, 1.0f);

}

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;

	ImGuiIO &io = ImGui::GetIO();
	io.DisplaySize.x = width;
	io.DisplaySize.y = height;
	io.DeltaTime = 1.0f / 60.0f;
	io.IniFilename = "imgui.ini";

	io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDLK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDLK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDLK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDLK_DOWN;
	io.KeyMap[ImGuiKey_Home] = SDLK_HOME;
	io.KeyMap[ImGuiKey_End] = SDLK_END;
	io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = SDLK_a;
	io.KeyMap[ImGuiKey_C] = SDLK_c;
	io.KeyMap[ImGuiKey_V] = SDLK_v;
	io.KeyMap[ImGuiKey_X] = SDLK_x;
	io.KeyMap[ImGuiKey_Y] = SDLK_y;
	io.KeyMap[ImGuiKey_Z] = SDLK_z;

	io.RenderDrawListsFn = render_gui;

	const void *png_data;
	uint png_size;
	ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
	load_texture_from_mem(tex_font, png_data, png_size, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

	if (!shader_tex.load_from_file("../data/shaders/texture.vs", "../data/shaders/texture.fs"))
		return false;

	if (!load_texture_from_file(tex_test, "../data/fonts/proggytinyttsz_8x12.png"))
		return false;
	return true;
}

void free_game()
{
	ImGui::Shutdown();
}

void init_game()
{
	quad = gen_tex_quad();	
}

void update_game(float dt)
{
	ImGuiIO &io = ImGui::GetIO();
}

void render_game(float dt)
{
	ImGui::NewFrame();
	ImGui::Text("Hello world!");
	ImGui::Render();

	use_shader(shader_tex);
	use_texture_2D(tex_test);
	uniform("tex", 0);
	quad.draw();
}

void on_key_up(uint16 mod, SDL_Keycode key) 
{
	//ImGuiIO &io = ImGui::GetIO();
	//io.KeysDown[key] = false;
	//io.KeyCtrl = (mod & KMOD_CTRL) != 0;
	//io.KeyShift = (mod & KMOD_SHIFT) != 0;
}

void on_key_down(uint16 mod, SDL_Keycode key) 
{
	//ImGuiIO &io = ImGui::GetIO();
	//io.KeysDown[key] = true;
	//io.KeyCtrl = (mod & KMOD_CTRL) != 0;
	//io.KeyShift = (mod & KMOD_SHIFT) != 0;
}

void on_text_input(char c[32])
{

}

void on_mouse_moved(int x, int y, int dx, int dy) 
{
	ImGuiIO &io = ImGui::GetIO();
	io.MousePos.x = x;
	io.MousePos.y = y;
}

void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) 
{
	ImGuiIO &io = ImGui::GetIO();
	if (button <= 2)
		io.MouseDown[button - 1] = true;
}

void on_mouse_released(int button, int x, int y) 
{
	ImGuiIO &io = ImGui::GetIO();
	if (button <= 2)
		io.MouseDown[button - 1] = false;
}