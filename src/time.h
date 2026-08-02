#pragma once
#include <SDL3/SDL.h>

// (Time namespace shamelessly stolen from Blah framework)
// https://github.com/NoelFB/blah
namespace Time
{
	// Ticks per second (microseconds in this case)
	constexpr Uint64 ticksPerSecond = 1000000;

	// Uptime (in ticks) at the start of the current frame
	extern Uint64 ticks;

	// Uptime (in seconds) at the start of the current frame
	extern double seconds;

	// Delta time from last frame
	extern float delta;

	// Delta time multiplier (use this for framerate independent game logic)
	extern float deltaTime;

	// Previous frame time (in ticks)
	extern Uint64 previousTicks;

	// Previous frame uptime (in seconds) 
	extern double previousSeconds;

	// Time the application should pause for
	extern float pauseTimer;

	// Uptime (in ticks). Polls the platform for an immediate value, unlike the cached 'Time::ticks' value
	Uint64 getTicks();

	// Pauses the entire application for the given time
	void pauseFor(float duration);

	// Returns true on the given interval
	bool onInterval(double time, float delta, float interval, float offset);
	bool onInterval(float delta, float interval, float offset);
	bool onInterval(float interval, float offset = 0);

	// Returns true when the given timestamp is passed
	bool onTime(double time, double timeStamp);

	// Returns true between time intervals
	bool betweenInterval(double time, float interval, float offset);
	bool betweenInterval(float interval, float offset = 0);
}

class Stopwatch
{
public:
	Stopwatch();
	void reset();
	Uint64 microseconds() const;
	Uint64 milliseconds() const;

private:
	Uint64 startTime;
};