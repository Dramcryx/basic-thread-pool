#include "threadpool.h"

void ThreadPool::run()
{
    while (!stop)
    {
        std::unique_lock<std::mutex> lock(sync_lock);
        if (!jobs.empty())
        {
            auto f = std::move(jobs.front());
            jobs.pop_front();
            lock.unlock();
            f();
        }
        else
        {
            lock.unlock();
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(100ms);
        }
    }
}

ThreadPool::ThreadPool(unsigned count)
{
    workers.reserve(count);
    for (auto i = 0u; i < count; ++i)
    {
        workers.emplace_back(&ThreadPool::run, this);
    }
}

ThreadPool::~ThreadPool()
{
    while (!jobs.empty());
    stop = true;
    for (auto &i: workers)
    {
        if (i.joinable())
        {
            i.join();
        }
    }
}
