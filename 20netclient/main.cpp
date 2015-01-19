#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 300
#include "opengl.h"
#include "fileio.h"
#include "shader.h"
#include "log.h"
#include "kyoko.h"
#include "../21netserver/net.h"
#include "../21netserver/win32_net.cpp"

bool GameLoad()
{
	
}

void GameUpdateAndRender(float dt)
{

}

void HandleEvents(bool &running, SDL_Window *window)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				running = false;
			break;
		case SDL_QUIT:
			running = false;
			break;
		}
	}
}

double GetElapsedTime(uint64 begin, uint64 end)
{
	uint64 f = SDL_GetPerformanceFrequency();
	return (double)(end - begin) / (double)f;
}

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		APP_LOG << "Failed to initialize SDL: " << SDL_GetError() << '\n';
		return EXIT_FAILURE;
	}

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
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);

	if (window == NULL)
	{
		APP_LOG << "Failed to create a window: " << SDL_GetError() << '\n';
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1); // Wait for vertical refresh
	SDL_ShowCursor(0);

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

	if(!GameLoad())
	{
		APP_LOG << "Failed to load content" << '\n';
		SDL_GL_DeleteContext(gl_context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	uint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	bool running = true;
	double target_frame_time = 1.0 / (double)CL_UPDATERATE;
	float timestep = 1.0f / (float)CL_UPDATERATE;
	uint64 update_begin = SDL_GetPerformanceCounter();
	while (running)
	{
		HandleEvents(running, window);
		GameUpdateAndRender(timestep);
		SDL_GL_SwapWindow(window);

		uint64 update_end = SDL_GetPerformanceCounter();
		double elapsed_update_time = GetElapsedTime(update_begin, update_end);
		if (elapsed_update_time < target_frame_time)
		{
			SDL_Delay(uint32(1000.0 * (target_frame_time - elapsed_update_time)));
			update_end = SDL_GetPerformanceCounter();
			elapsed_update_time = GetElapsedTime(update_begin, update_end);
		}
		update_begin = update_end;
	}

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}