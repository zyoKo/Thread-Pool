#include <future>
#include <iostream>

#include "ThreadPool/ThreadPool.h"

#include <mutex>
#include <thread>

std::mutex globalMutex;
int i = 0;

void function()
{
    std::lock_guard<std::mutex> lock(globalMutex);
	std::cout << "Execution on another thread: " << i << std::endl;
    ++i;
}

class Class1
{
public:
	void Update()
	{
        std::lock_guard<std::mutex> lock(class1Mutex);
		std::cout << "Update Class1: " << i << "\n";
        ++i;
	}

private:
    std::mutex class1Mutex;

    int i = 0;
};

void Checksum(const std::uint32_t num, std::atomic_uint64_t* checksum)
{
    *checksum += num;
}

int main()
{
	Threads::ThreadPool thread_pool{4};
    std::queue<std::future<void>> results;

    Class1 t;

    for (std::uint32_t n = 0; n < 1000; ++n)
    {
        results.emplace(thread_pool.CreateTask([&t] { t.Update(); }));
    }
    while (!results.empty())
    {
        results.front().get();
        results.pop();
    }

    /*std::packaged_task<void()> task(function);

	const auto future = task.get_future();

	std::thread thread(std::move(task));

	future.wait_for(std::chrono::seconds(1));

	thread.join();*/

    return 0;
}