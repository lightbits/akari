#include "timer.h"
#include <SDL.h>
#ifdef _WIN32
#include <Windows.h>
#endif

void sleep(double seconds)
{
	SDL_Delay((unsigned int)(seconds * 1000.0));
}

void sleep_ms(unsigned int ms)
{
	SDL_Delay(ms);
}

double get_elapsed_time()
{
	// Windows specific code
	// doesn't actually give good res

	return SDL_GetTicks() / 1000.0;
}