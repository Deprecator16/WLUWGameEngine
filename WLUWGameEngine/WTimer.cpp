#include "WTimer.h"

WTimer::WTimer()
{
	// Initialize timer
	startTicks = 0;
	pausedTicks = 0;

	started = false;
	paused = false;
}

void WTimer::start()
{
	started = true;
	paused = false;
	startTicks = SDL_GetTicks();
	pausedTicks = 0;
}

void WTimer::stop()
{
	started = false;
	paused = false;
	startTicks = 0;
	pausedTicks = 0;
}

void WTimer::pause()
{
	if (started && !paused)
	{
		//Pause the timer
		paused = true;

		//Calculate the paused ticks
		pausedTicks = SDL_GetTicks() - startTicks;
		startTicks = 0;
	}
}

void WTimer::unpause()
{
	if (started && paused)
	{
		//Unpause the timer
		paused = false;

		//Reset the starting ticks
		startTicks = SDL_GetTicks() - pausedTicks;

		//Reset the paused ticks
		pausedTicks = 0;
	}
}

Uint32 WTimer::getTicks()
{
	//The actual timer time
	Uint32 time = 0;

	//If the timer is running
	if (started)
	{
		//If the timer is paused
		if (paused)
		{
			//Return the number of ticks when the timer was paused
			time = pausedTicks;
		}
		else
		{
			//Return the current time minus the start time
			time = SDL_GetTicks() - startTicks;
		}
	}

	return time;
}

bool WTimer::isStarted()
{
	return started;
}

bool WTimer::isPaused()
{
	return paused && started;
}