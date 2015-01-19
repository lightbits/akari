/*
Useful links: 
http://blog.hvidtfeldts.net/index.php/2015/01/path-tracing-3d-fractals/
http://blog.hvidtfeldts.net/index.php/2012/10/stereographic-quaternion-julias/
http://people.cs.kuleuven.be/~philip.dutre/GI/TotalCompendium.pdf
*/

#include "opengl.h"
#include "fileio.h"
#include "shader.h"
#include "log.h"

const int WINDOW_WIDTH = 320;
const int WINDOW_HEIGHT = 240;

bool LoadShaders()
{
	return true;
}

void SaveScreenshot(SDL_Window *window)
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
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

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

	if(!LoadShaders())
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

	uint32 iteration = 0;
	uint64 perf_frequency = SDL_GetPerformanceFrequency();
	bool running = true;
	
	SDL_Event event = {};
	while (event.type != SDL_QUIT)
	{
		SDL_PollEvent(&event);
		switch (event.type)
		{
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_SPACE)
			{
				uint64 tick_begin = SDL_GetPerformanceCounter();

				//DoPathtraceIteration();
				SDL_Delay(16);
				SDL_GL_SwapWindow(window);

				if (check_gl_errors())
					break;

				uint64 tick_end = SDL_GetPerformanceCounter();
				double elapsed_time = (double)(tick_end - tick_begin) / (double)perf_frequency;

				char title[256];
				sprintf(title, "Iteration %d (%.2f ms)", iteration, 1000.0 * elapsed_time);
				SDL_SetWindowTitle(window, title);
				iteration++;
			}
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				running = false;
			if (event.key.keysym.sym == SDLK_PRINTSCREEN)
				SaveScreenshot(window);
			break;
		}
	}

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return EXIT_SUCCESS;
}