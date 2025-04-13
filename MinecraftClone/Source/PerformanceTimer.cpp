#include "PerformanceTimer.hpp"

PerformanceTimer::PerformanceTimer(const std::string& name)
	: name(name), start(std::chrono::high_resolution_clock::now()), stopped(false)
{

}

PerformanceTimer::~PerformanceTimer()
{
	if (!stopped)
	{
		stop();
	}
}

void PerformanceTimer::stop()
{
	end = std::chrono::high_resolution_clock::now();
	std::chrono::duration duration = end - start;
	float milliseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() * 0.001f;
	std::cout << "CPU time for " << name << ": " << milliseconds << std::endl;

	stopped = true;
}