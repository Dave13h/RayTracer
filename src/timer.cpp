//  _____ _
// |_   _(_)
//   | |  _ _ __ ___   ___ _ __
//   | | | | '_ ` _ \ / _ \ '__|
//   | | | | | | | | |  __/ |
//   \_/ |_|_| |_| |_|\___|_|
// 							  Class

#include "timer.h"

// ------------------------------------------------------------------------------------------------
// Timer Class
// ------------------------------------------------------------------------------------------------
cTimer::cTimer(const cTimer::timerState_t tState) {
	state = tState;
	dt = 0.0;

	now = SDL_GetPerformanceCounter();
	prev = now;
	cpuFreq = SDL_GetPerformanceFrequency();
}

cTimer::~cTimer(void) {
}

//
// Control Functions ------------------------------------------
//
cTimer *cTimer::start(void) {
	state = RUNNING;
	return this;
}
cTimer *cTimer::stop(void) {
	state = STOPPED;
	return this;
}

//
// Update Functions -------------------------------------------
//
cTimer *cTimer::tick(void) {
	if (state != RUNNING)
		return this;

	now = SDL_GetPerformanceCounter();
	dt = (now - prev) / (float) cpuFreq;
	prev = now;
	return this;
}

