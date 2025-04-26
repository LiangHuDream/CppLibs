#pragma once

class ThreadSafeCounter {
public:
    ThreadSafeCounter() {}
    ~ThreadSafeCounter() {}

    ThreadSafeCounter(const ThreadSafeCounter&) = delete;
    ThreadSafeCounter& operator=(const ThreadSafeCounter&) = delete;
    ThreadSafeCounter(ThreadSafeCounter&&) = delete;
    ThreadSafeCounter& operator=(ThreadSafeCounter&&) = delete;

    void Increase() {
        std::lock_guard<std::mutex> lk(mtx_);
        count_++;
    }

    void Clear() {
        std::lock_guard<std::mutex> lk(mtx_);
        count_ = 0;
    }

    int Get() const {
        std::lock_guard<std::mutex> lk(mtx_);

        return count_;
    }

private:
    std::mutex mtx_;
    int count_{};
};
