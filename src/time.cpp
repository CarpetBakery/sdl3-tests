#include "time.h"

// Init namespace vars
Uint64 Time::ticks = 0;
Uint64 Time::previousTicks = 0;
double Time::seconds = 0;
double Time::previousSeconds = 0;
float Time::delta = 0;
float Time::deltaTime = 0;
float Time::pauseTimer = 0;

Uint64 Time::getTicks()
{
	// Just use SDL's tick system
	auto counter = SDL_GetPerformanceCounter();
	auto perSecond = (double)SDL_GetPerformanceFrequency();
	return (Uint64)(counter * (Time::ticksPerSecond / perSecond));
}

void Time::pauseFor(float duration)
{
	if (duration >= pauseTimer)
		pauseTimer = duration;
}

bool Time::onInterval(double time, float delta, float interval, float offset)
{
	auto last = static_cast<long>((time - offset - delta) / interval);
	auto next = static_cast<long>((time - offset) / interval);
	return last < next;
}

bool Time::onInterval(float delta, float interval, float offset)
{
	return Time::onInterval(Time::seconds, delta, interval, offset);
}

bool Time::onInterval(float interval, float offset)
{
	return Time::onInterval(Time::seconds, Time::delta, interval, offset);
}

bool Time::onTime(double time, double timeStamp)
{
	float c = static_cast<float>(time) - Time::delta;
	return time >= timeStamp && c < timeStamp;
}

bool Time::betweenInterval(double time, float interval, float offset)
{
	static const auto modf = [](double x, double m)
	{
		return x - (int)(x / m) * m;
	};

	return modf(time - offset, ((double)interval) * 2) >= interval;
}

bool Time::betweenInterval(float interval, float offset)
{
	return betweenInterval(Time::seconds, interval, offset);
}

Stopwatch::Stopwatch()
{
	reset();
}

void Stopwatch::reset()
{
	startTime = Time::getTicks();
}

Uint64 Stopwatch::microseconds() const
{
	return Time::getTicks() - startTime;
}

Uint64 Stopwatch::milliseconds() const
{
	return microseconds() / 1000;
}
