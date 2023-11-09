#ifndef __MIRA_FIT_THREAD_POOL__
#define __MIRA_FIT_THREAD_POOL__

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

namespace mira
{
    class FitThreadPool
    {
    public:
        FitThreadPool(int n_threads);
        virtual ~FitThreadPool();

        template <class T, class... Args>
        void enqueue(T &&f, Args &&...args);

    protected:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;

        std::mutex queueMutex;
        std::condition_variable condition;
        bool stop;
    };
};

#endif /**__MIRA_FIT_THREAD_POOL__**/