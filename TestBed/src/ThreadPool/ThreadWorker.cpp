#include <iostream>

#include "ThreadWorker.h"

#include "ThreadPool.h"

namespace Threads
{
	ThreadWorker::ThreadWorker(ThreadPool* pool)
		:	threadPool(pool)
	{
	}

	void ThreadWorker::operator()() const
	{
		std::unique_lock<std::mutex> lock(threadPool->mutex);
		std::cout << "Starting operation\n";

		while(!threadPool->requestShutdown || 
			(threadPool->requestShutdown && !threadPool->taskQueue.empty()))
		{
			threadPool->busyThreads--;
			threadPool->conditionVariable.wait(lock, [this]()
			{
				return this->threadPool->requestShutdown || !this->threadPool->taskQueue.empty();
			});
			threadPool->busyThreads++;

			if (!threadPool->taskQueue.empty())
			{
				auto function = threadPool->taskQueue.front();
				threadPool->taskQueue.pop();

				lock.unlock();
				function();
				lock.lock();
			}
		}
	}
}
