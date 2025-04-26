#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <memory> // 引入智能指针头文件

// 定义消息类型
const int PRIORITY_MESSAGE_TYPE = 1;

// 消息结构体
struct Message {
    int id;
    int type;
};

// 有界缓冲区类
class BoundedBuffer {
private:
    std::queue<Message> buffer;
    std::mutex mtx;
    std::condition_variable not_full;
    std::condition_variable not_empty;
    size_t capacity;

public:
    BoundedBuffer(size_t cap) : capacity(cap) {}

    // 生产者向缓冲区添加消息
    void enqueue(const Message& msg) {
        std::unique_lock<std::mutex> lock(mtx);
        not_full.wait(lock, [this] { return buffer.size() < capacity; });
        buffer.push(msg);
        not_empty.notify_one();
    }

    // 消费者从缓冲区取出消息
    Message dequeue() {
        std::unique_lock<std::mutex> lock(mtx);
        not_empty.wait(lock, [this] { return!buffer.empty(); });

        Message msg = buffer.front();
        buffer.pop();
        not_full.notify_one();
        return msg;
    }
};

// 生产者任务类
class Producer {
public:
    Producer(std::shared_ptr<BoundedBuffer> buffer, int id)
        : buffer_(std::move(buffer)), id_(id) {}

    void operator()() {
        for (int i = 0; i < 5; ++i) {
            int messageType = (i % 2 == 0) ? PRIORITY_MESSAGE_TYPE : 2;
            Message msg{id_ * 10 + i, messageType};
            buffer_->enqueue(msg);
            std::cout << "Producer " << id_ << " produced message " << msg.id << " of type " << messageType << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

private:
    std::shared_ptr<BoundedBuffer> buffer_;
    int id_;
};

// 消费者任务类
class Consumer {
public:
    Consumer(std::shared_ptr<BoundedBuffer> buffer, int id)
        : buffer_(std::move(buffer)), id_(id) {}

    void operator()() {
        for (int i = 0; i < 5; ++i) {
            Message msg = buffer_->dequeue();
            std::cout << "Consumer " << id_ << " consumed message " << msg.id << " of type " << msg.type << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }

private:
    std::shared_ptr<BoundedBuffer> buffer_;
    int id_;
};

int main() {
    const int bufferCapacity = 10;
    auto buffer = std::make_shared<BoundedBuffer>(bufferCapacity);

    const int numProducers = 3;
    const int numConsumers = 2;

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    // 创建生产者线程
    for (int i = 0; i < numProducers; ++i) {
        producers.emplace_back(Producer(buffer, i));
    }

    // 创建消费者线程
    for (int i = 0; i < numConsumers; ++i) {
        consumers.emplace_back(Consumer(buffer, i));
    }

    // 等待生产者线程结束
    for (auto& p : producers) {
        p.join();
    }

    // 等待消费者线程结束
    for (auto& c : consumers) {
        c.join();
    }

    return 0;
}