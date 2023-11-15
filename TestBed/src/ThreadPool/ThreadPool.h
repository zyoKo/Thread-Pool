#pragma once

#include <functional>
#include <thread>
#include <future>
#include <condition_variable>
#include <queue>

namespace Threads
{
	class ThreadPool
	{
	public:
		ThreadPool(int size);
		~ThreadPool();

		ThreadPool(ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		ThreadPool& operator=(ThreadPool&) = delete;
		ThreadPool&& operator=(ThreadPool&&) = delete;

		void Shutdown();

		template <typename F, typename... Args>
		auto CreateTask(F&& func, Args&&... args) -> std::future<decltype(func(args...))>
		{
			auto taskPtr = std::make_shared<std::packaged_task<decltype(func(args...))()>>(
				std::bind(std::forward<F>(func), std::forward<Args>(args)...));

			auto wrapperFunction = [taskPtr]() { (*taskPtr)(); };
			{
			    std::lock_guard<std::mutex> lock(mutex);
			    taskQueue.push(wrapperFunction);
			    conditionVariable.notify_one();
			}

			return taskPtr->get_future();
		}

		size_t TaskQueueSize() noexcept;

		int GetNumberOfBusyThreads() const noexcept;

		void PrintThreadIds();

	private:
		std::mutex mutex;
		std::condition_variable conditionVariable;

		std::vector<std::thread> threads;
		int busyThreads;

		std::queue<std::function<void()>> taskQueue;

		bool requestShutdown;

		friend class ThreadWorker;
	};
}
