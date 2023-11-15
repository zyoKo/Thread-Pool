#pragma once

namespace Threads
{
	class ThreadPool;
}

namespace Threads
{
	class ThreadWorker
	{
	public:
		ThreadWorker(ThreadPool* pool);

		void operator()() const;

	private:
		ThreadPool* threadPool;
	};
}