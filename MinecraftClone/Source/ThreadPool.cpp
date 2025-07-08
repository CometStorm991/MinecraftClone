#include "ThreadPool.hpp"

ThreadPool::ThreadPool(uint32_t threadCount)
	: threadCount(threadCount), stopRequested(false)
{
	for (int i = 0; i < threadCount; i++)
	{
		threads.emplace_back(&ThreadPool::work, this);
	}
}

void ThreadPool::enqueue(const std::function<void(void)>& task)
{
	std::unique_lock<std::mutex> lock(mutex);
	taskQueue.push(task);
	lock.unlock();
	conditionVar.notify_one();
}

void ThreadPool::work()
{
	std::unique_lock<std::mutex> lock(mutex);
	while (true)
	{
		conditionVar.wait(lock, [this]() {
			return stopRequested || !taskQueue.empty();
			});

		if (stopRequested && taskQueue.empty())
		{
			return;
		}

		std::function<void(void)> task = taskQueue.front();
		taskQueue.pop();
		lock.unlock();

		task();
		lock.lock();
	}
}

void ThreadPool::stopAndWait()
{
	stopRequested = true;
	conditionVar.notify_all();
	for (int i = 0; i < threads.size(); i++)
	{
		if (threads.at(i).joinable())
		{
			threads.at(i).join();
		}
	}
}