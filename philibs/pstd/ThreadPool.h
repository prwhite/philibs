#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
public:
    ThreadPool(size_t = std::thread::hardware_concurrency());
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;

    // wait for the completion of all tasks
    // (meaning that all worker are waiting for new tasks)
    void wait() const;

private:
    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // the task queue
    std::queue< std::function<void()> > tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;

    // waiting for completion
    size_t active_worker;
    mutable std::mutex work_done_mutex;
    mutable std::condition_variable work_done_condition;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
    : stop(false),
    active_worker(threads)
{
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back(
            [this] {
                for (;;)
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);

                    --this->active_worker;

                    while (!this->stop && this->tasks.empty()) {
                        this->work_done_condition.notify_one(); // signal that this thread is done
                        this->condition.wait(lock); // and wait for more tasks
                    }

                    if (this->stop && this->tasks.empty())
                        return;

                    ++this->active_worker;

                    std::function<void()> task(this->tasks.front());
                    this->tasks.pop();
                    lock.unlock();

                    task();
                }
            }
        );
    }
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    // don't allow enqueueing after stopping the pool
    if (stop)
        throw std::runtime_error("enqueue on stopped ThreadPool");

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
}

inline void ThreadPool::wait() const {
    std::unique_lock<std::mutex> lock(work_done_mutex);

    // wait until all threads are done and tasks are empty
    while (!(active_worker == 0 && tasks.empty()))
        work_done_condition.wait(lock);
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        stop = true;
    }

    condition.notify_all();
    for (size_t i = 0; i < workers.size(); ++i)
        workers[i].join();
}

#endif
