#ifndef BLIB_SINGLE_TQ_TP_H
#define BLIB_SINGLE_TQ_TP_H

// 单任务队列线程池
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace blib {

    // 线程安全队列
    template<typename T>
    class SafeQueue {
    public:
        void push(const T &item) {
            {
                std::scoped_lock lock(mtx_);
                queue_.push(item);
            }
            cond_.notify_one();
        }

        void push(T &&item) {// 两个push方法，此处不是万能引用而是单纯右值
            {
                std::scoped_lock lock(mtx_);
                queue_.push(std::move(item));
            }
            cond_.notify_one();
        }

        bool pop(T &item) {
            std::unique_lock lock(mtx_);
            cond_.wait(lock, [&]() {
                return !queue_.empty() || stop_;
            });
            if (queue_.empty())
                return false;
            item = std::move(queue_.front());
            queue_.pop();
            return true;
        }

        std::size_t size() const {
            std::scoped_lock lock(mtx_);
            return queue_.size();
        }

        bool empty() const {
            std::scoped_lock lock(mtx_);
            return queue_.empty();
        }

        void stop() {
            {
                std::scoped_lock lock(mtx_);
                stop_ = true;
            }
            cond_.notify_all();
        }

    private:
        std::condition_variable cond_;
        mutable std::mutex mtx_;
        std::queue<T> queue_;
        bool stop_ = false;
    };

    using WorkItem = std::function<void()>;


    // 简易多线程单任务队列线程池，使用 SafeQueue 线程安全队列
    class SimplePool {
    public:
        explicit SimplePool(size_t threads = std::thread::hardware_concurrency()) {
            for (size_t i = 0; i < threads; ++i) {
                workers_.emplace_back([this]() {
                    for (;;) {
                        std::function<void()> task;
                        if (!queue_.pop(task))
                            return;

                        if (task)
                            task();
                    }
                });
            }
        }

        void enqueue(WorkItem item) {
            queue_.push(std::move(item));
        }

        ~SimplePool() {
            queue_.stop();
            for (auto &thd: workers_)
                thd.join();
        }

    private:
        SafeQueue<WorkItem> queue_;
        std::vector<std::thread> workers_;
    };

    class ThreadPool {
    public:
        explicit ThreadPool(size_t threads = std::thread::hardware_concurrency()) : stop(false) {
            // 根据threads数量创建多个线程
            for (size_t i = 0; i < threads; ++i) {
                workers.emplace_back([this]() {
                    for (;;) {// 工作线程就是一个死循环，不停查询任务队列并取出任务执行
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock,
                                                 [this]() {
                                                     return this->stop || !this->tasks.empty();// 条件变量等待线程池不为空或者stop
                                                 });
                            if (this->stop && this->tasks.empty())// 线程池为空且stop，证明线程池结束，退出线程
                                return;
                            task = std::move(this->tasks.front());// 取出任务
                            this->tasks.pop();
                        }

                        task();// 执行任务
                    }
                });// lambda表达式构建
            }
        }

        template<typename F, typename... Args>
        auto enqueue(F &&f, Args &&...args) {
            using return_type = std::invoke_result_t<F, Args...>;
            auto task = std::make_shared<std::packaged_task<return_type()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...));// 完美转发，构造任务仿函数的指针
            std::future<return_type> res = task->get_future();                  // 获得函数执行的future返回
            {
                std::unique_lock<std::mutex> lock(queue_mutex);

                if (stop) {
                    throw std::runtime_error("enqueue on stopped Thread pool");
                }

                tasks.emplace([task = std::move(task)]() { (*task)(); });// 塞入任务队列
            }                                                            // 入队列后即可解锁
            condition.notify_one();                                      // 仅唤醒一个线程，避免无意义的竞争
            return res;
        }

        ~ThreadPool() {
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                stop = true;
            }
            condition.notify_all();// 唤醒所有线程，清理任务
            for (std::thread &worker: workers)
                worker.join();// 阻塞，等待所有线程执行结束
        }

    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;
    };
}


#endif
