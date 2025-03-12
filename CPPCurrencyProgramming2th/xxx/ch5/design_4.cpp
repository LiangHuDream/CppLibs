#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

class AtomicReadWriteLock {
private:
    std::atomic<int> readers;
    std::atomic<bool> writer;

public:
    AtomicReadWriteLock() : readers(0), writer(false) {}

    // 加读锁
    void readLock() {
        while (true) {
            // 检查是否有写操作正在进行
            while (writer.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }
            // 尝试增加读者计数
            int oldReaders = readers.load(std::memory_order_relaxed);
            if (readers.compare_exchange_strong(oldReaders, oldReaders + 1, std::memory_order_acq_rel)) {
                // 再次检查是否有写操作
                if (writer.load(std::memory_order_acquire)) {
                    // 如果有写操作，减少读者计数并继续尝试
                    readers.fetch_sub(1, std::memory_order_relaxed);
                } else {
                    break;
                }
            }
        }
    }

    // 解读锁
    void readUnlock() {
        readers.fetch_sub(1, std::memory_order_release);
    }

    // 加写锁
    void writeLock() {
        while (true) {
            // 尝试获取写锁
            if (!writer.exchange(true, std::memory_order_acq_rel)) {
                // 检查是否有读者
                while (readers.load(std::memory_order_acquire) > 0) {
                    std::this_thread::yield();
                }
                break;
            }
            std::this_thread::yield();
        }
    }

    // 解写锁
    void writeUnlock() {
        writer.store(false, std::memory_order_release);
    }
};

// 读操作线程函数
void readOperation(AtomicReadWriteLock& lock, int id) {
    lock.readLock();
    std::cout << "Reader " << id << " is reading." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Reader " << id << " finished reading." << std::endl;
    lock.readUnlock();
}

// 写操作线程函数
void writeOperation(AtomicReadWriteLock& lock, int id) {
    lock.writeLock();
    std::cout << "Writer " << id << " is writing." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "Writer " << id << " finished writing." << std::endl;
    lock.writeUnlock();
}

int main() {
    AtomicReadWriteLock lock;
    std::vector<std::thread> threads;

    // 创建读线程
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(readOperation, std::ref(lock), i);
    }

    // 创建写线程
    threads.emplace_back(writeOperation, std::ref(lock), 0);

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    return 0;
}