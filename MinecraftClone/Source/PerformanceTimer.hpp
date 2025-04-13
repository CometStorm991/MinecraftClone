#pragma once

#include <chrono>
#include <iostream>
#include <string>

class PerformanceTimer
{
private:
	std::string name;
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point end;

	bool stopped;
public:
	PerformanceTimer(const std::string& name);
	~PerformanceTimer();

	void stop();
};