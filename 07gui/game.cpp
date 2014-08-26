#include "game.h"
#include "sdlgui.h"

int
	window_width,
	window_height;

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;

	return true;
}

void free_game()
{

}

void init_game()
{

}

void update_game(float dt)
{

}

void render_game(float dt)
{
	clear(0xcc9999ff, 1.0f);
	ImGui::NewFrame();
	ImGui::ShowTestWindow();

	static float lightColor[4];
	static float attenuation;
	ImGui::Begin("Diffuse Shader");
	ImGui::ColorEdit4("lightColor", lightColor);
	ImGui::SliderFloat("attenuation", &attenuation, 1.0f, 16.0f);
	ImGui::End();
	ImGui::Render();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }