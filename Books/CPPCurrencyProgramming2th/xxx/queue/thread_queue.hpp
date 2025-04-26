#pragma once

#include <queue>
#include <mutex>
#include <fcntl.h>

template <typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() {}
    ~ThreadSafeQueue() {}

    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue(ThreadSafeQueue&&) = delete;
    ThreadSafeQueue& operator=(ThreadSafeQueue&&) = delete;

    void Push(T value) {
        std::lock_guard<std::mutex> lk(queueMutex_);
        que_.push(value);
    }

    void Pop() {
        std::lock_guard<std::mutex> lk(queueMutex_);
        if (que_.empty()) {
            std::err << "queue empty, cannot pop" << std::endl;
        }

        que_.pop();
    }
    
    T& Front() {
        std::lock_guard<std::mutex> lk(queueMutex_);
        if (que_.empty()) {
            std::err << "queue empty, cannot return front" << std::endl;
            return {}; // 悬空引用
        }

        return que_.front();
    }

    bool Empty() const {
        std::lock_guard<std::mutex> lk(queueMutex_);
        return que_.empty();
    }

private:
    std::queue<T> que_{};
    std::mutex queueMutex_{};
};

