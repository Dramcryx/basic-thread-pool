#ifndef THREAD_POOL
#define THREAD_POOL

#include <thread>
#include <mutex>
#include <list>
#include <vector>
#include <functional>


class ThreadPool
{
public:
    ThreadPool(unsigned count = std::thread::hardware_concurrency());
    ~ThreadPool();

    template <class F>
    void submit(F && f)
    {
        std::lock_guard<std::mutex> lock(sync_lock);
        jobs.emplace_back(std::forward<F>(f));
    }

    template <class F, typename ...Args>
    void submit(F && f, Args&&... args)
    {
        std::lock_guard<std::mutex> lock(sync_lock);
        jobs.push_back(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    }

private:
    std::vector<std::thread> workers;
    std::list<std::function<void()>> jobs;
    std::mutex sync_lock;
    bool stop = false;

    void run();
};

#endif
