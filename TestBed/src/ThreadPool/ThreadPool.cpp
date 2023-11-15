#include "ThreadPool.h"

#include <iostream>

#include "ThreadWorker.h"

namespace Threads
{
	ThreadPool::ThreadPool(int size)
		:	threads(std::vector<std::thread>(size)),
			busyThreads(size),
			requestShutdown(false)
	{
		for (int i = 0; i < size; ++i)
		{
			threads[i] = std::thread(ThreadWorker(this));
		}
	}

	ThreadPool::~ThreadPool()
	{
		Shutdown();
	}

	void ThreadPool::Shutdown()
	{
		{
			std::lock_guard<std::mutex> lock(mutex);
			requestShutdown = true;
			conditionVariable.notify_all();
		}

		for (auto& thread : threads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}
	}

	size_t ThreadPool::TaskQueueSize() noexcept
	{
		std::unique_lock<std::mutex> lock(mutex);
		return taskQueue.size();
	}

	int ThreadPool::GetNumberOfBusyThreads() const noexcept
	{
		return busyThreads;
	}

	void ThreadPool::PrintThreadIds()
	{
		std::unique_lock<std::mutex> lock(mutex);
		for (const auto& thread : threads)
		{
			std::cout << thread.get_id() << std::endl;
		}
	}
}
