#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
private:
	uint32_t threadCount;
	std::vector<std::thread> threads;

	std::mutex mutex;
	std::condition_variable conditionVar;
	std::queue<std::function<void(void)>> taskQueue;
	std::atomic<bool> stopRequested;
	std::condition_variable completion;

	void work();
public:
	ThreadPool(uint32_t threadCount);

	void enqueue(const std::function<void(void)>& task);
	void stopAndWait();
};