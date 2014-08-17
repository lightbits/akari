#include "opengl.h"
#include "timer.h"
#include "fileio.h"
#include "shader.h"
#include "log.h"
#include "game.h"
#include <iostream>
#include <fstream>
#include <sstream>

void print_debug_info()
{
	int context_flags, accelerated, double_buffered, depth_size, stencil_size, samples;
	int red_size, green_size, blue_size, alpha_size;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &context_flags);
	SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &accelerated);
	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &double_buffered);
	SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth_size);
	SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil_size);
	SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &samples);
	SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &red_size);
	SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &green_size);
	SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &blue_size);
	SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &alpha_size);
	APP_LOG << "Debug context: " << (context_flags & SDL_GL_CONTEXT_DEBUG_FLAG ? "yes" : "no") << '\n';
	APP_LOG << "HW accelerated: " << (accelerated ? "yes" : "no") << '\n';
	APP_LOG << "Double buffered: " << (double_buffered ? "yes" : "no") << '\n';
	APP_LOG << "FSAA samples: " << samples << '\n';
	APP_LOG << "Depth bits: " << depth_size << '\n';
	APP_LOG << "Stencil bits: " << stencil_size << '\n';
	APP_LOG << "RGBA bits: " << red_size << " " << green_size << " " << blue_size << " " << alpha_size << '\n';
	APP_LOG << "Vendor: " << glGetString(GL_VENDOR) << '\n';
	APP_LOG << "Renderer: " << glGetString(GL_RENDERER) << '\n';
	APP_LOG << "GL ver.: " << glGetString(GL_VERSION) << '\n';
	APP_LOG << "GLSL ver.: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
}

void save_screenshot(SDL_Window *window)
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	uint8 *pixels = new uint8[width * height * 3];
	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

	for (int y = 0; y < height / 2; y++)
	for (int x = 0; x < width * 3; x++)
		std::swap(pixels[y * width * 3 + x], pixels[(height - 1 - y) * width * 3 + x]);

	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(pixels, width, height, 8 * 3, width * 3, 0, 0, 0, 0);
	SDL_SaveBMP(surface, "screenshot.bmp");
	SDL_FreeSurface(surface);
	delete[] pixels;
}

void handle_events(bool &running, SDL_Window *window)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				running = false;
			if (event.key.keysym.sym == SDLK_PRINTSCREEN)
				save_screenshot(window);
			on_key_up(event.key.keysym.mod, event.key.keysym.sym);
			break;
		case SDL_KEYDOWN:
			on_key_down(event.key.keysym.mod, event.key.keysym.sym);
			break;
		case SDL_MOUSEMOTION:
			if (event.motion.state & SDL_BUTTON_LMASK)
				on_mouse_dragged(SDL_BUTTON_LEFT, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
			else if (event.motion.state & SDL_BUTTON_MMASK)
				on_mouse_dragged(SDL_BUTTON_MIDDLE, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
			else if (event.motion.state & SDL_BUTTON_RMASK)
				on_mouse_dragged(SDL_BUTTON_RIGHT, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
			else
				on_mouse_moved(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
			break;
		case SDL_MOUSEBUTTONDOWN:
			on_mouse_pressed(event.button.button, event.button.x, event.button.y);
			break;
		case SDL_MOUSEBUTTONUP:
			on_mouse_released(event.button.button, event.button.x, event.button.y);
			break;
		case SDL_QUIT:
			running = false;
			break;
		}
	}
}

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		APP_LOG << "Failed to initialize SDL: " << SDL_GetError() << '\n';
		return EXIT_FAILURE;
	}

	int window_width = 640;
	int window_height = 480;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	SDL_Window *window = SDL_CreateWindow(
		"Sample Application",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width, window_height,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);

	if (window == NULL)
	{
		APP_LOG << "Failed to create a window: " << SDL_GetError() << '\n';
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1); // Wait for vertical refresh
	//SDL_ShowCursor(0);

	glewExperimental = true;
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		APP_LOG << "Failed to load OpenGL functions: " << glewGetErrorString(glew_error) << '\n';
		SDL_GL_DeleteContext(gl_context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	print_debug_info();

	if(!load_game(window_width, window_height))
	{
		APP_LOG << "Failed to load content" << '\n';
		SDL_GL_DeleteContext(gl_context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	init_game();

	uint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glViewport(0, 0, window_width, window_height);

	double frame_time = 0.0;
	bool running = true;
	while (running)
	{
		double frame_begin = get_elapsed_time();
		handle_events(running, window);
		update_game(frame_time);
		render_game(frame_time);
		SDL_GL_SwapWindow(window);
		frame_time = get_elapsed_time() - frame_begin;

		if (check_gl_errors())
			running = false;
	}

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}