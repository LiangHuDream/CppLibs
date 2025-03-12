#include <iostream>
#include <atomic>
#include <vector>
#include <thread>
#include <chrono>

template<typename T>
class ThreadSafeCircularBuffer {
private:
    std::vector<T> buffer;
    std::atomic<size_t> readIndex;
    std::atomic<size_t> writeIndex;
    const size_t capacity;

    // 计算下一个索引
    size_t nextIndex(size_t index) const {
        return (index + 1) % capacity;
    }

public:
    // 构造函数，初始化缓冲区
    ThreadSafeCircularBuffer(size_t cap) : buffer(cap), readIndex(0), writeIndex(0), capacity(cap) {}

    // 写入数据的方法
    bool write(const T& value) {
        size_t currentWriteIndex = writeIndex.load(std::memory_order_relaxed);
        size_t next = nextIndex(currentWriteIndex);

        // 检查缓冲区是否已满
        if (next == readIndex.load(std::memory_order_acquire)) {
            return false; // 缓冲区已满
        }

        // 写入数据
        buffer[currentWriteIndex] = value;
        writeIndex.store(next, std::memory_order_release);
        return true;
    }

    // 读取数据的方法
    bool read(T& result) {
        size_t currentReadIndex = readIndex.load(std::memory_order_relaxed);

        // 检查缓冲区是否为空
        if (currentReadIndex == writeIndex.load(std::memory_order_acquire)) {
            return false; // 缓冲区为空
        }

        // 读取数据
        result = buffer[currentReadIndex];
        readIndex.store(nextIndex(currentReadIndex), std::memory_order_release);
        return true;
    }

    // 获取缓冲区的容量
    size_t getCapacity() const {
        return capacity;
    }

    // 检查缓冲区是否为空
    bool isEmpty() const {
        return readIndex.load(std::memory_order_relaxed) == writeIndex.load(std::memory_order_relaxed);
    }

    // 检查缓冲区是否已满
    bool isFull() const {
        return nextIndex(writeIndex.load(std::memory_order_relaxed)) == readIndex.load(std::memory_order_relaxed);
    }
};

// 测试写入线程函数
void writer(ThreadSafeCircularBuffer<int>& buffer, int id) {
    for (int i = 0; i < 5; ++i) {
        while (!buffer.write(i + id * 5)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::cout << "Writer " << id << " wrote " << i + id * 5 << std::endl;
    }
}

// 测试读取线程函数
void reader(ThreadSafeCircularBuffer<int>& buffer, int id) {
    int value;
    for (int i = 0; i < 5; ++i) {
        while (!buffer.read(value)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::cout << "Reader " << id << " read " << value << std::endl;
    }
}

int main() {
    ThreadSafeCircularBuffer<int> buffer(10);

    std::vector<std::thread> writers;
    std::vector<std::thread> readers;

    // 创建写入线程
    for (int i = 0; i < 2; ++i) {
        writers.emplace_back(writer, std::ref(buffer), i);
    }

    // 创建读取线程
    for (int i = 0; i < 2; ++i) {
        readers.emplace_back(reader, std::ref(buffer), i);
    }

    // 等待所有写入线程完成
    for (auto& t : writers) {
        t.join();
    }

    // 等待所有读取线程完成
    for (auto& t : readers) {
        t.join();
    }

    return 0;
}