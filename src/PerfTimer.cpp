// ----------------------------------------------------
// Fast timer with microsecond precision
// ----------------------------------------------------

#include "PerfTimer.h"

Uint64 PerfTimer::frequency = SDL_GetPerformanceFrequency();

// Fill Constructor, Start(), ReadMs() and ReadTicks() methods
// they are simple, one line each!

PerfTimer::PerfTimer()
{
	Start();
}

void PerfTimer::Start()
{
	startTime = SDL_GetPerformanceCounter();
}

double PerfTimer::ReadMs() const
{
	return (double)(SDL_GetPerformanceCounter() - startTime) / frequency * 1000.0;
}

Uint64 PerfTimer::ReadTicks() const
{
	return SDL_GetPerformanceCounter() - startTime;
}
