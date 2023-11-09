#include "mira_FitThreadPool.hpp"

mira::FitThreadPool(int n_threads) : stop(false)
{
    for (int i = 0; i < n_threads; ++i)
    {
        workers.emplace_back([this]
                             {
                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });

                        if (stop && tasks.empty()) {
                            return;
                        }

                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    task();
                } });
    }
}

mira::~FitThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }

    condition.notify_all();

    for (std::thread &worker : workers)
    {
        worker.join();
    }
}

template <class T, class... Args>
void mira::FitThreadPool::enqueue(T &&f, Args &&...args)
{
    std::function<void()> task = std::bind(std::forward<T>(f), std::forward<Args>(args)...);

    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (stop)
        {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        tasks.emplace(std::move(task));
    }

    condition.notify_one();
}