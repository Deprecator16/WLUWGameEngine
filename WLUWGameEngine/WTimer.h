#pragma once

#include "Header.h"

class WTimer
{
public:
	// Constructor
	WTimer();

	// Management
	void start();
	void stop();
	void pause();
	void unpause();

	// Information
	Uint32 getTicks();

	// Status
	bool isStarted();
	bool isPaused();

private:
	Uint32 startTicks;
	Uint32 pausedTicks;

	bool started;
	bool paused;
};

