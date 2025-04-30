/*
 * Author: LiangHuDream
 * Date: 2025-04-30
 * Copyright © 2025 LiangHuDream. All rights reserved.
 * License: MIT
 * Description: 线程池的简单实现: 线程池的创建和初始化, 任务提交,线程池的终止, 等待完成
 */
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t threads) : stop(false) {
        for(size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                while(true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                        });
                        if(this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template<class F, class... Args>
    void enqueue(F&& f, Args&&... args) {
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if(stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");
            tasks.emplace(task);
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for(std::thread &worker : workers)
            worker.join();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};

int main() {
    ThreadPool pool(4);
    std::mutex cout_mutex;
    int done = 0;

    for(int i = 0; i < 8; ++i) {
        pool.enqueue([i, &done, &cout_mutex] {
            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "Task " << i << " is running\n";
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                done++;
                std::cout << "Task " << i << " done. Total done: " << done << std::endl;
            }
        });
    }

    // 等待足够时间让所有任务完成，实际中可能需要更精确的同步
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 0;
}