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
#ifdef _WIN32
	static __int64 freq = 0; // ticks per sec

	if(freq == 0)
	{
		LARGE_INTEGER freqTemp;
		QueryPerformanceFrequency(&freqTemp);
		freq = freqTemp.QuadPart;
	}

	LARGE_INTEGER ticks;
	QueryPerformanceCounter(&ticks);

	// Conversion to double shouldn't be a problem unless you have an uptime of a couple
	// of decades
	return (double)(ticks.QuadPart / (double)freq);
#else
	return SDL_GetTicks() / 1000.0;
#endif
}